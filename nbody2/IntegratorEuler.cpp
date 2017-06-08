#include "Error.h"
#include "IntegratorEuler.h"
#include "Vector.h"

#include <sstream>

namespace nbody
{
	IntegratorEuler::IntegratorEuler(IModel * model, double step)
		: IIntegrator(model, step),
		m_state(nullptr)
	{
		if (!model)
			throw MAKE_ERROR("Model was nullptr");

		m_state = new Vector2d[model->getDim()];
		m_k1 = new Vector2d[model->getDim()];

		std::stringstream ss;
		ss << "Euler (dt =" << step << ")";
		this->setName(ss.str());
	}

	IntegratorEuler::~IntegratorEuler()
	{
		delete[] m_state;
	}

	std::unique_ptr<IIntegrator> IntegratorEuler::create(IModel * model, double step)
	{
		return std::make_unique<IntegratorEuler>(model, step);
	}

	void IntegratorEuler::singleStep()
	{
		m_model->eval(m_state, m_time, m_k1);

		for (size_t i = 1; i < m_dim; i++)
			m_state[i] += m_step * m_k1[i];

		m_time += m_step;
	}

	void IntegratorEuler::setInitialState(Vector2d * state)
	{
		for (size_t i = 0; i < m_dim; i++)
			m_state[i] = state[i];

		m_time = 0;
	}

	Vector2d * IntegratorEuler::getState() const
	{
		return m_state;
	}
}
