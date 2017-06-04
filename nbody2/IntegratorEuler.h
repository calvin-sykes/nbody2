#ifndef INTEGRATOR_EULER_H
#define INTEGRATOR_EULER_H

#include "IIntegrator.h"

namespace nbody
{
	class IntegratorEuler : public IIntegrator
	{
	public:
		static std::unique_ptr<IIntegrator> create(IModel * model, double step);
		
		IntegratorEuler(IModel * model, double step);
		virtual ~IntegratorEuler();

		virtual void singleStep();
		virtual void setInitialState(Vector2d * state);
		virtual Vector2d * getState() const;

	private:
		Vector2d * m_state;
	};
}

#endif // !INTEGRATOR_EULER_H

