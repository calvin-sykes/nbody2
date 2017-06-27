#ifndef INTEGRATOR_ADB2_H
#define INTEGRATOR_ADB2_H

#include "IIntegrator.h"

namespace nbody
{
	class IntegratorADB2 :
		public IIntegrator
	{
	public:
		static std::unique_ptr<IIntegrator> create(IModel * model, double step);

		IntegratorADB2(IModel * model, double step);
		virtual ~IntegratorADB2();

		void singleStep() override;
		void setInitialState(Vector2d* state) override;
		Vector2d const* getStateVector() const override;

	private:
		Vector2d * m_state;
		Vector2d * m_f[2];
	};
}

#endif // INTEGRATOR_ADB2_H