#include "Error.h"
#include "IIntegrator.h"

namespace nbody
{
	IIntegrator::IIntegrator(IModel * model, double step)
		: m_model(model),
		m_step(step),
		m_time(0),
		m_n_steps(0),
		m_dim(model ? model->getDim() : 0)
	{
		if (!model)
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

	size_t IIntegrator::getNumSteps() const
	{
		return m_n_steps;
	}

	double IIntegrator::getTime() const
	{
		return m_time;
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
}