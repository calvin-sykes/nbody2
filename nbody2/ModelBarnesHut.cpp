#include "BodyGroupProperties.h"
#include "Constants.h"
#include "IDistributor.h"
#include "ModelBarnesHut.h"
#include "Timings.h"
#include "Types.h"

#include <algorithm>

#include <iostream>

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
		ParticleData all{ state, m_aux_state, deriv_state };

		timings[Timings::TREE_BUILD_START] = Clock::now();
		calcBounds(all);
		buildTree(all);
		timings[Timings::TREE_BUILD_END] = Clock::now();

		timings[Timings::FORCE_CALC_START] = Clock::now();
		m_root.calcForces();
		for (auto i = 0; i < m_num_bodies; i++)
		{
			deriv_state[i].vel = state[i].vel;
		}
		timings[Timings::FORCE_CALC_END] = Clock::now();
	}

	BHTreeNode const* ModelBarnesHut::getTreeRoot() const
	{
		return &m_root;
	}

	void ModelBarnesHut::calcBounds(ParticleData const & all)
	{
        auto static len_mult_fact = 1;
        
        auto num = this->getNumBodies();
        auto num_renegades = m_root.getNumRenegades();
        auto frac_renegade = static_cast<double>(num_renegades) / num;
        std::cout << frac_renegade << std::endl;
        
        if(frac_renegade > 0.01)
            len_mult_fact++;
            
        auto avg_dist = std::accumulate(&all.m_state[0].pos,
                                        &all.m_state[m_num_bodies - 1].pos,
                                        0.0,
                                        [](auto const& a, auto const& b) { return a + b.mag(); }) / num;
        
        /*auto furthest = std::max_element(&all.m_state[0].pos,
										 &all.m_state[m_num_bodies - 1].pos,
										 [](auto const& a, auto const& b) { return a.mag_sq() < b.mag_sq(); });
                                         
		auto len = (*furthest - m_centre_mass).mag();*/
		
        m_bounds = Quad{ m_centre_mass, len_mult_fact * avg_dist };
	}

	void ModelBarnesHut::buildTree(ParticleData const & all)
	{
		m_root.reset(m_bounds);

		for (size_t i = 0; i < m_num_bodies; i++)
		{
			// extract data for a single particle
			ParticleData p{ &all.m_state[i], &all.m_aux_state[i], &all.m_deriv_state[i] };

			m_root.insert(p);
		}

		m_root.computeMassDistribution();
		m_root.threadTree();

		m_centre_mass = m_root.getCentreMass();

	}
}
