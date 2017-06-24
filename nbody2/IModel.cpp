#include "BodyGroupProperties.h"
#include "Constants.h"
#include "IDistributor.h"
#include "IColourer.h"
#include "IModel.h"
#include "Timings.h"

namespace nbody
{
	IModel::IModel(std::string name, bool has_tree, size_t dim)
		: m_initial_state(nullptr),
		m_aux_state(nullptr),
		m_colour_state(nullptr),
		m_masked(nullptr),
		m_step(1),
		m_num_bodies(0),
		m_num_added(0),
		m_tot_mass(0),
		m_centre_mass(),
		m_has_tree(has_tree),
		m_dim(dim),
		m_name(name)
	{
	}

	IModel::~IModel()
	{
		delete[] m_initial_state;
		delete[] m_aux_state;
		delete[] m_colour_state;
		delete[] m_masked;

	}

	void IModel::init(size_t num_bodies, double step)
	{
		resetDim(num_bodies, step);
	}

	void IModel::addBodies(IDistributor const & dist, std::unique_ptr<IColourer> col, BodyGroupProperties const & bgp)
	{
		ParticleData bodies{ m_initial_state + m_num_added, m_aux_state + m_num_added };
		dist.createDistribution(bodies, bgp);

		col->setup(m_num_added, bgp.num, bgp.cols, &bodies);
		m_colourers.emplace_back(std::move(col));

		for (auto i = m_num_added; i < m_num_added + bgp.num; i++)
		{
			m_centre_mass += m_initial_state[i].pos * m_aux_state[i].mass;
			m_tot_mass += m_aux_state[i].mass;
		}

		m_num_added += bgp.num;
		m_centre_mass /= m_tot_mass;
	}

	void IModel::updateColours(Vector2d const * state)
	{
		for (auto& col : m_colourers)
		{
			col->apply(reinterpret_cast<ParticleState const*>(state), m_aux_state, m_colour_state);
		}
	}

	bool IModel::hasTree() const
	{
		return m_has_tree;
	}

	size_t IModel::getNumBodies() const
	{
		return m_num_bodies;
	}

	ParticleAuxState const* IModel::getAuxState() const
	{
		return m_aux_state;
	}

	ParticleColourState const* IModel::getColourState() const
	{
		return m_colour_state;
	}

	Vector2d IModel::getCentreMass() const
	{
		return m_centre_mass;
	}

	double IModel::getTotalEnergy(Vector2d const * all) const
	{
		auto ke = 0.0, pe = 0.0;
		auto ps = reinterpret_cast<ParticleState const *>(all);

		timings[Timings::ENERGY_CALC_START] = Clock::now();
#pragma omp parallel for schedule(static) reduction(+:pe,ke)
		for (int i = 0; i < m_num_bodies; i++)
		{
			ke += 0.5 * m_aux_state[i].mass * ps[i].vel.mag_sq();

			for (auto j = 0; j < m_num_bodies; j++)
			{
				if (j == i)
					continue;
				auto rel_pos_mag = (ps[j].pos - ps[i].pos).mag();
				pe += -m_aux_state[j].mass * m_aux_state[i].mass * Constants::G / rel_pos_mag;
			}

		}
		timings[Timings::ENERGY_CALC_END] = Clock::now();

		return ke + pe;
	}

	size_t IModel::getDim() const
	{
		return m_dim;
	}

	void IModel::setDim(size_t const dim)
	{
		m_dim = dim;
	}

	std::string const & IModel::getName() const
	{
		return m_name;
	}

	void IModel::resetDim(size_t num_bodies, double step)
	{
		m_num_bodies = num_bodies;
		setDim(num_bodies * 2); // position and velocity for each body

		m_initial_state = new ParticleState[num_bodies];
		m_aux_state = new ParticleAuxState[num_bodies];
		m_colour_state = new ParticleColourState[num_bodies];
		m_masked = new bool[num_bodies];

		for (auto i = 0; i < num_bodies; i++)
			m_masked[i] = false;

		m_step = step;
	}

	Vector2d * IModel::getInitialStateVector() const
	{
		return reinterpret_cast<Vector2d *>(m_initial_state);
	}
}
