#include "BodyGroupProperties.h"
#include "Constants.h"
#include "IDistributor.h"
#include "ModelBarnesHut.h"
#include "Timings.h"
#include "Types.h"

#include <algorithm>
#include <numeric>

namespace nbody
{
	ModelBarnesHut::ModelBarnesHut()
		: IModel("Barnes-Hut N-body simulation", true),
		m_root(m_bounds),
		m_bounds({ 0, 0 }, 0)
	{
	}

	ModelBarnesHut::~ModelBarnesHut()
	{
	}

	std::unique_ptr<IModel> ModelBarnesHut::create()
	{
		return std::make_unique<ModelBarnesHut>();
	}

	void ModelBarnesHut::eval(Vector2d * state_in, double time, Vector2d * deriv_out)
	{
		// Reinterpret single array of vectors as individual particles
		// Simplifies following logic
		auto state{ reinterpret_cast<ParticleState *>(state_in) };
		auto deriv_state{ reinterpret_cast<ParticleDerivState *>(deriv_out) };
		ParticleData all{ state, m_aux_state };

		timings[Timings::TREE_BUILD_START] = Clock::now();
		calcBounds(all);
		buildTree(all);
		timings[Timings::TREE_BUILD_END] = Clock::now();

		timings[Timings::FORCE_CALC_START] = Clock::now();
#pragma omp parallel for schedule(static)
		for (auto i = 1; i < m_num_bodies; i++)
		{
			if (m_masked[i])
				continue;

			ParticleData p{ &state[i], &m_aux_state[i] };

			deriv_state[i].acc = m_root.calcForce(p);
			deriv_state[i].vel = state[i].vel;
		}

		// particle 0 calculated separately for stats
		m_root.forceCalcStatReset();

		ParticleData p{ &state[0], &m_aux_state[0] };

		deriv_state[0].acc = m_root.calcForce(p);
		deriv_state[0].vel = state[0].vel;
		timings[Timings::FORCE_CALC_END] = Clock::now();
	}

	BHTreeNode const* ModelBarnesHut::getTreeRoot() const
	{
		return &m_root;
	}

	void ModelBarnesHut::calcBounds(ParticleData const & all)
	{
		auto static len_mult_fact = 4;

		/*auto avg_dist_from_centre = std::accumulate(
			&all.m_state[0].pos,
			&all.m_state[m_num_bodies - 1].pos,
			0.,
			[this](double a, Vector2d const& b) { return a + (b - this->m_centre_mass).mag(); });*/
		auto avg_dist_from_centre = priv::accumulate_if(
			&all.m_state[0],
			&all.m_state[m_num_bodies - 1],
			0.0,
			[this](double a, ParticleState const& b) { return a + (b.pos - this->m_centre_mass).mag(); },
			[all, this](ParticleState & a) {return !this->m_masked[std::distance(&all.m_state[0], &a)]; });
		avg_dist_from_centre /= m_num_bodies;

		auto furthest_from_centre = std::max_element(
			&all.m_state[0],
			&all.m_state[m_num_bodies - 1],
			[](ParticleState const& a, ParticleState const& b) { return a.pos.mag_sq() < b.pos.mag_sq(); });

		auto idx = std::distance(all.m_state, furthest_from_centre);

		// If this body is not already known to be escaping
		if (m_masked[idx] == false && m_root.getQuad().contains(furthest_from_centre->pos) == false)
		{
			auto test_pos = furthest_from_centre->pos;
			auto test_vel = furthest_from_centre->vel;
			auto test_mass = m_aux_state[idx].mass;

			// Calculate energy to determine if body is escaping
			auto ke = 0.5 * test_mass * test_vel.mag_sq();
			auto pe = 0.0;
			for (auto i = 0; i < m_num_bodies; i++)
			{
				if (i == idx)
					continue;
				auto rel_pos_mag = (test_pos - all.m_state[i].pos).mag();
				pe += -m_aux_state[i].mass * test_mass * Constants::G / rel_pos_mag;
			}

			if (ke + pe > 0)
			{
				// body is escaping - we ignore it when considering enlarging the tree
				m_masked[idx] = true;
			}
			else
			{
				// body is not escaping so enlarge the tree
				len_mult_fact++;
			}
		}

		auto len = len_mult_fact * avg_dist_from_centre;
		m_bounds = Quad{ m_centre_mass, len };
	}

	void ModelBarnesHut::buildTree(ParticleData const & all)
	{
		m_root.reset(m_bounds);

		for (size_t i = 0; i < m_num_bodies; i++)
		{
			// extract data for a single particle
			ParticleData p{ &(all.m_state[i]), &(all.m_aux_state[i]) };

			m_root.insert(p, 0);
		}

		m_root.computeMassDistribution();

		m_centre_mass = m_root.getCentreMass();

	}
}
