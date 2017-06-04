#include "Error.h"
#include "IIntegrator.h"
#include "Vector.h"

namespace nbody
{
	IIntegrator::IIntegrator(IModel * model, double step)
		: m_model(model), m_step(step), m_time(0), m_err(0), m_dim(model ? model->getDim() : 0)
	{
		if(!model)
			throw MAKE_ERROR("Model was nullptr");

		if (step <= 0)
			throw MAKE_ERROR("Step was <= 0");
	}

	IIntegrator::~IIntegrator()
	{
	}

	void IIntegrator::setStepSize(double step)
	{
		m_step = step;
	}

	double IIntegrator::getStepSize() const
	{
		return m_step;
	}

	double IIntegrator::getTime() const
	{
		return m_time;
	}

	double IIntegrator::getError() const
	{
		return m_err;
	}

	void IIntegrator::setModel(IModel * model)
	{
		if (!model)
			throw MAKE_ERROR("Model was nullptr");
		else
		{
			m_model = model;
		}
	}

	IModel * IIntegrator::getModel() const
	{
		return m_model;
	}
	void IIntegrator::setName(std::string id)
	{
		m_name = id;
	}

	std::string const& IIntegrator::getName() const
	{
		return m_name;
	}

	void IIntegrator::eval(Vector2d const* initial,
		Vector2d const* deriv_in,
		double step,
		double time,
		Vector2d * deriv_out)
	{
		auto state = new Vector2d[m_dim];

		for (size_t i = 0; i < m_model->getDim(); i++)
			state[i] = initial[i] + step * deriv_in[i];

		m_model->eval(state, time + step, deriv_out);

		delete[] state;
	}
}