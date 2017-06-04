#ifndef INTEGRATOR_EULER_IMPROVED_H
#define INTEGRATOR_EULER_IMPROVED_H

#include "IIntegrator.h"

namespace nbody
{
	class IntegratorEulerImproved :	public IIntegrator
	{
	public:
		static std::unique_ptr<IIntegrator> create(IModel * model, double step);

		IntegratorEulerImproved(IModel * model, double step);
		virtual ~IntegratorEulerImproved();

		virtual void singleStep();
		virtual void setInitialState(Vector2d * state);
		virtual Vector2d * getState() const;

	private:
		Vector2d * m_state, * m_tmp, * m_k1, * m_k2;
	};
}

#endif // INTEGRATOR_EULER_IMPROVED_H

