#ifndef INTEGRATOR_ADB6_H
#define INTEGRATOR_ADB6_H

#include "IIntegrator.h"

namespace nbody
{
	class IntegratorADB6 :
		public IIntegrator
	{
	public:
		static std::unique_ptr<IIntegrator> create(IModel * model, double step);

		IntegratorADB6(IModel * model, double step);
		virtual ~IntegratorADB6();

		void singleStep() override;
		void setInitialState(Vector2d* state) override;
		Vector2d const* getStateVector() const override;

	private:
		double static constexpr m_c[6] = { 4277.0 / 1440.0,
										  -7923.0 / 1440.0,
										   9982.0 / 1440.0,
										  -7298.0 / 1440.0,
										   2877.0 / 1440.0,
										  - 475.0 / 1440.0 };

		Vector2d * m_state;
		Vector2d * m_f[6];
	};
}

#endif // INTEGRATOR_ADB6_H