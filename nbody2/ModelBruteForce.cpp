#include "BodyDistributor.h"
#include "BodyGroupProperties.h"
#include "Constants.h"
#include "Error.h"
#include "ModelBruteForce.h"
#include "Types.h"

namespace nbody
{
	ModelBruteForce::ModelBruteForce()
		: IModel("Brute-force N-body simulation")
	{
	}

	ModelBruteForce::~ModelBruteForce()
	{
	}

	std::unique_ptr<IModel> ModelBruteForce::create()
	{
		return std::make_unique<ModelBruteForce>();
	}

	void ModelBruteForce::addBodies(BodyDistributor const & dist, BodyGroupProperties const & bgp)
	{
		ParticleData empty(m_initial_state + m_num_added, m_aux_state + m_num_added);
		dist.createDistribution(empty, bgp);

		for (size_t i = m_num_added; i < m_num_added + bgp.num; i++)
			m_tot_mass += m_aux_state[i].mass;

		m_num_added += bgp.num;
	}

	void ModelBruteForce::eval(Vector2d * state_in, double time, Vector2d * deriv_out)
	{
		// Reinterpret single array of vectors as individual particles
		// Simplifies following logic
		ParticleState* state = reinterpret_cast<ParticleState *>(state_in);
		ParticleDerivState* deriv_state = reinterpret_cast<ParticleDerivState *>(deriv_out);

		for (size_t i = 0; i < m_num_bodies; i++)
		{
			deriv_state[i].vel = {};
			deriv_state[i].acc = {};
		}

		m_centre_mass = {};

#pragma omp parallel for schedule(static)
		for (int i = 0; i < m_num_bodies; i++)
		{
			for (int j = i + 1; j < m_num_bodies; j++)
			{
				auto rel_pos = state[j].pos - state[i].pos; // relative position vector r
				auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
				auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // rhat = r/|r|
				// F = (G m1 m2 / (|r|**2 + eps**2) * r_hat
				// a = F / m1
				deriv_state[i].acc += (Constants::G * m_aux_state[j].mass / (rel_pos_mag_sq + Constants::SOFTENING * Constants::SOFTENING)) * unit_vec;
				deriv_state[j].acc -= (Constants::G * m_aux_state[i].mass / (rel_pos_mag_sq + Constants::SOFTENING * Constants::SOFTENING)) * unit_vec;

				deriv_state[i].vel = state[i].vel;
				deriv_state[j].vel = state[j].vel;
			}
		}

		for (size_t i = 0; i < m_num_bodies; i++)
			m_centre_mass += state[i].pos * m_aux_state[i].mass;

		m_centre_mass /= m_tot_mass;
	}

	BHTreeNode const* ModelBruteForce::getTreeRoot() const
	{
		return nullptr;
	}
}