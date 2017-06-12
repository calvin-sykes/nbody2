#include "BodyDistributor.h"
#include "BodyGroupProperties.h"
#include "Constants.h"
#include "ModelBarnesHut.h"
#include "Types.h"

#include <numeric>

namespace nbody
{
	ModelBarnesHut::ModelBarnesHut()
		: IModel("Barnes-Hut N-body simulation", true),
		m_root(m_bounds),
		m_bounds({ 0, 0 }, Constants::RADIUS)
	{
	}

	ModelBarnesHut::~ModelBarnesHut()
	{
	}

	std::unique_ptr<IModel> ModelBarnesHut::create()
	{
		return std::make_unique<ModelBarnesHut>();
	}

	/*void ModelBarnesHut::addBodies(BodyDistributor const & dist, BodyGroupProperties const & bgp)
	{
		ParticleData empty{ m_initial_state + m_num_added, m_aux_state + m_num_added };
		dist.createDistribution(empty, bgp);

		for (auto i = m_num_added; i < m_num_added + bgp.num; i++)
		{
			m_centre_mass += m_initial_state[i].pos * m_aux_state[i].mass;
			m_tot_mass += m_aux_state[i].mass;
		}

		m_num_added += bgp.num;
		m_centre_mass /= m_tot_mass;
	}*/

	void ModelBarnesHut::eval(Vector2d * state_in, double time, Vector2d * deriv_out)
	{
		// Reinterpret single array of vectors as individual particles
		// Simplifies following logic
		auto state{ reinterpret_cast<ParticleState *>(state_in) };
		auto deriv_state{ reinterpret_cast<ParticleDerivState *>(deriv_out) };
		ParticleData all{ state, m_aux_state };

		calcBounds(all);
		buildTree(all);

#pragma omp parallel for schedule(static)
		for (int i{ 1 }; i < m_num_bodies; i++)
		{
			ParticleData p{ &state[i], &m_aux_state[i] };

			deriv_state[i].acc = m_root.calcForce(p);
			deriv_state[i].vel = state[i].vel;
		}

		// particle 0 calculated separately for stats
		m_root.statReset();

		ParticleData p{ &state[0], &m_aux_state[0] };

		deriv_state[0].acc = m_root.calcForce(p);
		deriv_state[0].vel = state[0].vel;
	}

	BHTreeNode const* ModelBarnesHut::getTreeRoot() const
	{
		return &m_root;
	}

	void ModelBarnesHut::calcBounds(ParticleData const & all)
	{
		/*auto minmax_x{ std::minmax_element(&all.m_state[0].pos, &all.m_state[m_num_bodies].pos, [](Vector2d const& a, Vector2d const& b) -> bool {return a.x < b.x; }) };
		auto minmax_y{ std::minmax_element(&all.m_state[0].pos, &all.m_state[m_num_bodies].pos, [](Vector2d const& a, Vector2d const& b) -> bool {return a.y < b.y; }) };

		auto len_x{ minmax_x.second->x - minmax_x.first->x };
		auto len_y{ minmax_y.second->y - minmax_y.first->y };
		auto len{ std::max(len_x, len_y) };
		
		Vector2d centre{ (minmax_x.second->x + minmax_x.first->x) / 2, (minmax_y.second->y + minmax_y.first->y) / 2 };

		m_bounds = Quad{ centre.x, centre.y, len };*/

		auto avg_dist_from_centre = std::accumulate(&all.m_state[0].pos, &all.m_state[m_num_bodies].pos, double{ 0 }, [this](double a, Vector2d const& b) { return a + (b - this->m_centre_mass).mag(); });
		avg_dist_from_centre /= m_num_bodies;

		auto len{ 4 * avg_dist_from_centre };

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