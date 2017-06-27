#include "IntegratorADB2.h"

#include <sstream>

namespace nbody
{
	std::unique_ptr<IIntegrator> IntegratorADB2::create(IModel * model, double step)
	{
		return std::make_unique<IntegratorADB2>(model, step);
	}

	IntegratorADB2::IntegratorADB2(IModel * model, double step)
		: IIntegrator(model, step),
		m_state(nullptr),
		m_f{ nullptr, nullptr }
	{
		m_state = new Vector2d[m_dim];
		for (auto i = 0; i < 2; i++)
		{
			m_f[i] = new Vector2d[m_dim];
		}

		std::stringstream ss;
		ss << "Adams-Bashforth 2 step (dt =" << step << ")";
		setName(ss.str());
	}

	IntegratorADB2::~IntegratorADB2()
	{
		delete[] m_state;
		for (auto i = 0; i < 2; i++)
		{
			delete[] m_f[i];
		}
	}

	void IntegratorADB2::singleStep()
	{
		for (auto i = 0; i < m_dim; i++)
		{
			m_state[i] += m_step / 2.0 * (3.0 * m_f[1][i] - m_f[0][i]);

			m_f[0][i] = m_f[1][i];
		}

		m_time += m_step;
		m_n_steps++;

		m_model->eval(m_state, m_time, m_f[1]);
	}

	void IntegratorADB2::setInitialState(Vector2d * state)
	{
		for (auto i = 0; i < m_dim; i++)
		{
			m_state[i] = state[i];
		}

		m_time = 0;

		// RK4 for initialisation
		auto k1 = new Vector2d[m_dim];
		auto k2 = new Vector2d[m_dim];
		auto k3 = new Vector2d[m_dim];
		auto k4 = new Vector2d[m_dim];
		auto tmp = new Vector2d[m_dim];

		// k1
		m_model->eval(m_state, m_time, k1);
		for (auto i = 0; i < m_dim; i++)
		{
			tmp[i] = m_state[i] + m_step * 0.5 * k1[i];
		}

		// k2
		m_model->eval(tmp, m_time + m_step * 0.5, k2);
		for (auto i = 0; i < m_dim; i++)
		{
			tmp[i] = m_state[i] + m_step * 0.5 * k2[i];
		}

		// k3
		m_model->eval(tmp, m_time + m_step * 0.5, k3);
		for (auto i = 0; i < m_dim; i++)
		{
			tmp[i] = m_state[i] + m_step * k3[i];
		}

		// k4
		m_model->eval(tmp, m_time + m_step, k4);
		
		for (auto i = 0; i < m_dim; i++)
		{
			m_state[i] += m_step / 6.0 * (k1[i] + 2 * (k2[i] + k3[i]) + k4[i]);
			m_f[0][i] = k1[i];
			m_f[1][i] = k4[i];
		}

		m_time += m_step;
		m_n_steps++;

		delete[] k1;
		delete[] k2;
		delete[] k3;
		delete[] k4;
		delete[] tmp;
	}

	Vector2d const * IntegratorADB2::getStateVector() const
	{
		return m_state;
	}
}