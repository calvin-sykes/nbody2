#ifndef IINTEGRATOR_H
#define IINTEGRATOR_H

#include "IModel.h"

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
		}
		} };

	class IIntegrator
	{
	public:
		IIntegrator(IModel* model, double step);
		virtual ~IIntegrator();
		void setStepSize(double step);

		double getStepSize() const;
		double getTime() const;
		double getError() const;

		void setModel(IModel* model);
		IModel* getModel() const;

		void setName(std::string id);
		std::string const& getName() const;

		virtual void singleStep() = 0;
		virtual void setInitialState(Vector2d* state) = 0;
		virtual Vector2d const* getState() const = 0;

	protected:
		IModel* m_model;
		double m_step, m_time, m_err;
		size_t const m_dim;
		std::string m_name;

	private:
		IIntegrator(const IIntegrator& ref) = delete;
		IIntegrator& operator=(const IIntegrator& ref) = delete;
	};
}

#endif // !IINTEGRATOR_H
