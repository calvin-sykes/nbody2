#ifndef IINTEGRATOR_H
#define IINTEGRATOR_H

#include "IModel.h"

#include <memory>

namespace nbody
{
	enum class IntegratorType
	{
		EULER,
		MODIFIED_EULER,
		N_INTEGRATORS,
		INVALID = -1
	};

	struct IntegratorProperties
	{
		IntegratorType const type;
		char const* name;
	};

	class IIntegrator
	{
	public:
		IIntegrator(IModel * model, double step);
		virtual ~IIntegrator();
		void setStepSize(double step);
		
		double getStepSize() const;
		double getTime() const;
		double getError() const;
		
		void setModel(IModel * model);
		IModel * getModel() const;
		
		void setName(std::string id);
		std::string const& getName() const;

		virtual void singleStep() = 0;
		virtual void setInitialState(Vector2d * state) = 0;
		virtual Vector2d * getState() const = 0;

	protected:		
		void eval(Vector2d const* initial,
			Vector2d const* deriv_in,
			double step,
			double time,
			Vector2d * deriv_out);

		IModel * m_model;
		double m_step, m_time, m_err;
		size_t const m_dim;
		std::string m_name;

	private:
		IIntegrator(const IIntegrator &ref) = delete;
		IIntegrator& operator=(const IIntegrator &ref) = delete;
	};
}

#endif // !IINTEGRATOR_H