#ifndef INTEGRATOR_EULER_IMPROVED_H
#define INTEGRATOR_EULER_IMPROVED_H

#include "IIntegrator.h"

#include <memory>

namespace nbody
{
	class IntegratorEulerImproved :	public IIntegrator
	{
	public:
		static std::unique_ptr<IIntegrator> create(IModel * model, double step);

		IntegratorEulerImproved(IModel * model, double step);
		virtual ~IntegratorEulerImproved();

		void singleStep() override;
		void setInitialState(Vector2d * state) override;
		Vector2d const* getStateVector() const override;

	private:
		Vector2d * m_state, * m_tmp, * m_k1, * m_k2;
	};
}

#endif // INTEGRATOR_EULER_IMPROVED_H

