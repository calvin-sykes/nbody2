#pragma once

#include "Vector.h"
#include "Constants.h"

namespace nbody
{
	class Integrator
	{
	public:

		Integrator() = default;

		~Integrator() = default;

		inline virtual void step(const size_t id, Vector2d& pos, Vector2d& vel, const Vector2d& accel, const double dt) const = 0;
	};

	class EulerIntegrator : public Integrator
	{
	public:
		inline virtual void step(const size_t id, Vector2d& pos, Vector2d& vel, const Vector2d& accel, const double dt) const
		{
			vel += accel * dt;
			pos += vel * dt;
		}
	};
}