#ifndef INTEGRATOR_EULER_H
#define INTEGRATOR_EULER_H

#include <memory>

#include "IIntegrator.h"

namespace nbody
{
	class IntegratorEuler : public IIntegrator
	{
	public:
		static std::unique_ptr<IIntegrator> create(IModel * model, double step);
		
		IntegratorEuler(IModel * model, double step);
		virtual ~IntegratorEuler();

		void singleStep() override;
		void setInitialState(Vector2d * state) override;
		Vector2d const* getStateVector() const override;

	private:
		Vector2d * m_state, * m_k1;
	};
}

#endif // !INTEGRATOR_EULER_H

