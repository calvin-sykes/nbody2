#include "BodyDistributor.h"
#include "BodyGroupProperties.h"
#include "Constants.h"
#include "ModelBarnesHut.h"
#include "Types.h"

namespace nbody
{
	ModelBarnesHut::ModelBarnesHut()
		: IModel("Barnes-Hut N-body simulation", true),
		m_bounds(0, 0, Constants::RADIUS),
		m_root(m_bounds)
	{
	}

	ModelBarnesHut::~ModelBarnesHut()
	{
	}

	std::unique_ptr<IModel> ModelBarnesHut::create()
	{
		return std::make_unique<ModelBarnesHut>();
	}

	void ModelBarnesHut::addBodies(BodyDistributor const & dist, BodyGroupProperties const & bgp)
	{
		ParticleData empty(m_initial_state + m_num_added, m_aux_state + m_num_added);
		dist.createDistribution(empty, bgp);

		for (size_t i = m_num_added; i < m_num_added + bgp.num; i++)
		{
			m_centre_mass += m_initial_state[i].pos * m_aux_state[i].mass;
			m_tot_mass += m_aux_state[i].mass;
		}

		m_num_added += bgp.num;
		m_centre_mass /= m_tot_mass;
	}

	void ModelBarnesHut::eval(Vector2d * state_in, double time, Vector2d * deriv_out)
	{
		// Reinterpret single array of vectors as individual particles
		// Simplifies following logic
		ParticleState* state = reinterpret_cast<ParticleState *>(state_in);
		ParticleDerivState* deriv_state = reinterpret_cast<ParticleDerivState *>(deriv_out);
		ParticleData all(state, m_aux_state);

		// calcBounds()
		buildTree(all);

#pragma omp parallel for schedule(static)
		for (int i = 1; i < m_num_bodies; i++)
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