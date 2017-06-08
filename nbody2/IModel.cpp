#include "IModel.h"

namespace nbody
{
	IModel::IModel(std::string name, bool has_tree, size_t dim)
		: m_initial_state(nullptr),
		m_aux_state(nullptr),
		m_step(1),
		m_num_bodies(0),
		m_num_added(0),
		m_tot_mass(0),
		m_centre_mass(),
		m_has_tree(has_tree),
		m_dim(dim),
		m_name(name)
	{}

	IModel::~IModel()
	{
		delete m_initial_state;
		delete m_aux_state;
	}

	void IModel::init(size_t num_bodies, double step)
	{
		resetDim(num_bodies, step);
	}

	bool IModel::hasTree() const
	{
		return m_has_tree;
	}

	size_t IModel::getNumBodies() const
	{
		return m_num_bodies;
	}

	ParticleAuxState const * IModel::getAuxState() const
	{
		return m_aux_state;
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

		delete[] m_initial_state;
		m_initial_state = new ParticleState[num_bodies];
		m_aux_state = new ParticleAuxState[num_bodies];

		m_step = step;
	}

	Vector2d * IModel::getInitialState() const
	{
		return reinterpret_cast<Vector2d *>(m_initial_state);
	}
}