#include "BodyGroupProperties.h"
#include "BodyDistributor.h"
#include "IColourer.h"
#include "IModel.h"

namespace nbody
{
	IModel::IModel(std::string name, bool has_tree, size_t dim)
		: m_initial_state(nullptr),
		m_aux_state(nullptr),
		m_colour_state(nullptr),
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
		delete m_initial_state;
		delete m_aux_state;
		delete m_colour_state;
	}

	void IModel::init(size_t num_bodies, double step)
	{
		resetDim(num_bodies, step);
	}

	void IModel::addBodies(BodyDistributor const & dist, std::unique_ptr<IColourer> col, BodyGroupProperties const & bgp)
	{
		col->setup(m_num_added, bgp.num, bgp.cols);
		m_colourers.emplace_back(std::move(col));
		
		
		ParticleData empty(m_initial_state + m_num_added, m_aux_state + m_num_added);
		dist.createDistribution(empty, bgp);

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

	size_t IModel::getNumAdded() const
	{
		return m_num_added;
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

		m_step = step;
	}

	Vector2d * IModel::getInitialState() const
	{
		return reinterpret_cast<Vector2d *>(m_initial_state);
	}
}