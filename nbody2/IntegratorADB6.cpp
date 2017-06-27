#include "IntegratorADB6.h"

#include <sstream>

namespace nbody
{
	std::unique_ptr<IIntegrator> IntegratorADB6::create(IModel * model, double step)
	{
		return std::make_unique<IntegratorADB6>(model, step);
	}

	IntegratorADB6::IntegratorADB6(IModel * model, double step)
		: IIntegrator(model, step),
		m_state(nullptr),
		m_f{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }
	{
		m_state = new Vector2d[m_dim];
		for (auto i = 0; i < 6; i++)
		{
			m_f[i] = new Vector2d[m_dim];
		}

		std::stringstream ss;
		ss << "Adams-Bashforth 6 step (dt =" << step << ")";
		setName(ss.str());
	}

	IntegratorADB6::~IntegratorADB6()
	{
		delete[] m_state;
		for (auto i = 0; i < 6; i++)
		{
			delete[] m_f[i];
		}
	}

	void IntegratorADB6::singleStep()
	{
		for (auto i = 0; i < m_dim; i++)
		{
			m_state[i] += m_step * (m_c[0] * m_f[5][i] +
									m_c[1] * m_f[4][i] +
									m_c[2] * m_f[3][i] +
									m_c[3] * m_f[2][i] +
									m_c[4] * m_f[1][i] +
									m_c[5] * m_f[0][i] );

			m_f[0][i] = m_f[1][i];
			m_f[1][i] = m_f[2][i];
			m_f[2][i] = m_f[3][i];
			m_f[3][i] = m_f[4][i];
			m_f[4][i] = m_f[5][i];
		}

		m_time += m_step;
		m_n_steps++;

		m_model->eval(m_state, m_time, m_f[5]);
	}

	void IntegratorADB6::setInitialState(Vector2d * state)
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

		for (auto n = 0; n < 5; n++)
		{
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
				m_f[n][i] = k1[i];
			}

			m_time += m_step;
			m_n_steps++;
		}

		m_model->eval(m_state, m_time, m_f[5]);

		delete[] k1;
		delete[] k2;
		delete[] k3;
		delete[] k4;
		delete[] tmp;
	}

	Vector2d const * IntegratorADB6::getStateVector() const
	{
		return m_state;
	}
}