#include "Error.h"
#include "IntegratorEulerImproved.h"
#include "Vector.h"

#include <sstream>

namespace nbody
{
	std::unique_ptr<IIntegrator> IntegratorEulerImproved::create(IModel * model, double step)
	{
		return std::make_unique<IntegratorEulerImproved>(model, step);
	}

	IntegratorEulerImproved::IntegratorEulerImproved(IModel * model, double step)
		: IIntegrator(model, step),
		m_state(nullptr),
		m_tmp(nullptr),
		m_k1(nullptr),
		m_k2(nullptr)
	{
		if (!model)
			throw MAKE_ERROR("Model was nullptr");

		m_state = new Vector2d[model->getDim()];
		m_tmp = new Vector2d[model->getDim()];
		m_k1 = new Vector2d[model->getDim()];
		m_k2 = new Vector2d[model->getDim()];

		std::stringstream ss;
		ss << "Modified Euler (dt =" << step << ")";
		this->setName(ss.str());
	}

	IntegratorEulerImproved::~IntegratorEulerImproved()
	{
		delete[] m_state;
		delete[] m_tmp;
		delete[] m_k1;
		delete[] m_k2;
	}

	void IntegratorEulerImproved::singleStep()
	{
		m_model->eval(m_state, m_time, m_k1);

		for (size_t i = 0; i < m_dim; i++)
			m_tmp[i] = m_state[i] + m_step * m_k1[i];

		m_model->eval(m_tmp, m_time + m_step, m_k2);

		for (size_t i = 0; i < m_dim; i++)
			m_state[i] += (m_step / 2.0) * (m_k1[i] + m_k2[i]);

		m_time += m_step;
	}

	void IntegratorEulerImproved::setInitialState(Vector2d * state)
	{
		for (size_t i = 0; i < m_dim; i++)
			m_state[i] = state[i];

		m_time = 0;
	}

	Vector2d * IntegratorEulerImproved::getState() const
	{
		return m_state;
	}
}
