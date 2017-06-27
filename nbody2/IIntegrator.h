#ifndef IINTEGRATOR_H
#define IINTEGRATOR_H

#include "IModel.h"

namespace nbody
{
	enum class IntegratorType
	{
		EULER,
		MODIFIED_EULER,
		ADB2,
		ADB6, 
		N_INTEGRATORS,
		INVALID = -1
	};

	struct IntegratorProperties
	{
		constexpr IntegratorProperties(IntegratorType const type, char const* name)
			: type(type),
			  name(name)
		{
		}

		IntegratorType const type;
		char const* name;
	};

	using IntArray = std::array<IntegratorProperties, static_cast<size_t>(IntegratorType::N_INTEGRATORS)>;

	constexpr IntArray integrator_infos = { {
		{
			IntegratorType::EULER,
			"Euler"
		},
		{
			IntegratorType::MODIFIED_EULER,
			"Modified Euler"
		},
		{
			IntegratorType::ADB2,
			"Adams-Bashforth 2 step"
		},
		{
			IntegratorType::ADB6,
			"Adams-Bashforth 6 step"
		}
		} };

	class IIntegrator
	{
	public:
		IIntegrator(IModel* model, double step);
		virtual ~IIntegrator();
		void setStepSize(double step);

		double getStepSize() const;
		size_t getNumSteps() const;
		double getTime() const;

		void setModel(IModel* model);
		IModel* getModel() const;

		void setName(std::string id);
		std::string const& getName() const;

		virtual void singleStep() = 0;
		virtual void setInitialState(Vector2d* state) = 0;
		virtual Vector2d const* getStateVector() const = 0;

	protected:
		IModel* m_model;
		double m_step, m_time;
		size_t m_n_steps;
		size_t const m_dim;
		std::string m_name;

	private:
		IIntegrator(const IIntegrator& ref) = delete;
		IIntegrator& operator=(const IIntegrator& ref) = delete;
	};
}

#endif // !IINTEGRATOR_H
