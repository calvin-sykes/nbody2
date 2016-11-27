#pragma once

#include "Body2d.h"
#include "Constants.h"

#include <functional>
#include <random>
#include <chrono>

namespace nbody
{
	namespace detail
	{
		// Calculate the sign of a value
		// Returns -1 if val < 0, +1 if val > 0, and 0 if val == 0 
		template <typename T> int sgn(T val)
		{
			return (T(0) < val) - (val < T(0));
		}

		// Calculate the circular velocity at radius r around a mass central_mass
		Vector2d vCirc(const Vector2d& r_, const double central_mass)
		{
			auto mag_v = sqrt(G * central_mass / r_.mag());
			auto angle_v = atan(abs(r_.y / r_.x));
			auto v_x = -1 * sgn(r_.y) * sin(angle_v) * mag_v;
			auto v_y = sgn(r_.x) * cos(angle_v) * mag_v;
			return Vector2d(v_x, v_y);
		}
	}

	using namespace detail;

	class BodyDistributor
	{
	public:
		BodyDistributor(const double radiusIn, const double central_massIn, const Vector2d& pos_offIn = Vector2d(), const Vector2d& vel_offIn = Vector2d())
			: radius(radiusIn), central_mass(central_massIn), pos_offset(pos_offIn), vel_offset(vel_offIn)
		{
			std::default_random_engine gen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
			std::uniform_real_distribution<> dist(0, 1);
			get_rand = std::bind(dist, gen);
		}
		virtual Body2d sample() const = 0;
	protected:
		std::function<double(void)> get_rand;

		double radius, central_mass;
		Vector2d pos_offset, vel_offset;
	};

	class ExponentialDistributor : public BodyDistributor
	{
	private:
		// exponential distribution function
		double expDist(double lambda) const
		{
			return log(1 - get_rand()) / lambda;
		}
	public:
		ExponentialDistributor(const double radiusIn, const double central_massIn, const double lambdaIn, const Vector2d& pos_offIn = Vector2d(), const Vector2d& vel_offIn = Vector2d())
			: BodyDistributor(radiusIn, central_massIn, pos_offIn, vel_offIn), lambda(lambdaIn) {}

		virtual Body2d sample() const
		{
			auto actual_radius = radius * (expDist(lambda)) + EPS;
			auto phi = 2 * PI * get_rand();
			Vector2d pos(actual_radius * cos(phi), actual_radius * sin(phi));
			//auto pos_x = radius * expDist(lambda) * (0.5 - get_rand());
			//auto pos_y = radius * expDist(lambda) * (0.5 - get_rand());
			//Vector2d pos(pos_x, pos_y);

			//auto mass = (1 + get_rand()) * 10 * M_SOL;
			auto mass = pow(BODY_MASS / M_SOL, 0.5 + get_rand()) * M_SOL;

			return Body2d(pos + pos_offset, vCirc(pos, central_mass) + vel_offset, mass, true);
		}

	private:
		double lambda;
	};

	class UniformDistributor : public BodyDistributor
	{
	public:
		UniformDistributor(const double radiusIn, const double central_massIn, const Vector2d& pos_offIn = Vector2d(), const Vector2d& vel_offIn = Vector2d())
			: BodyDistributor(radiusIn, central_massIn, pos_offIn, vel_offIn) {}

		virtual Body2d sample() const
		{
			auto rad = EPS + (radius - EPS) * get_rand();
			//auto rad = EPS + radius * (1 - pow(get_rand(),2));
			auto theta = get_rand() * 2. * PI;
			auto pos_x = rad * cos(theta);
			auto pos_y = rad * sin(theta);

			Vector2d pos(pos_x, pos_y);

			auto mass = pow(BODY_MASS / M_SOL, 0.5 + get_rand()) * M_SOL;

			return Body2d(pos + pos_offset, vCirc(pos, central_mass) + vel_offset, mass, true);
		}
	};

	/*class IsothermalDistributor : public BodyDistributor
	{
	public:

		virtual Body2d sample() const
		{



			Vector2d pos(pos_x, pos_y);

			auto mass = (1. + get_rand()) * 10. * M_SOL;

			return Body2d(pos, vCirc(pos), mass, true);
		}
	};*/

	class PlummerDistributor : public BodyDistributor
	{
	public:

		PlummerDistributor(const double radiusIn, const double central_massIn, const size_t nIn, const Vector2d& pos_offIn = Vector2d(), const Vector2d& vel_offIn = Vector2d())
			: BodyDistributor(radiusIn, central_massIn, pos_offIn, vel_offIn), n(nIn) {}

		virtual Body2d sample() const
		{

			auto mass = 10 * M_SOL;
			auto m_tot = n * mass;
			// Calculate radius as function of total mass enclosed inside that radius
			// which is chosen randomly
			auto rad = RAD_PL / sqrt(pow(get_rand(), -2.0f / 3.0f) - 1.);
			// Random distribution for azimuthal angle
			auto theta = 2. * PI * get_rand();
			auto pos = Vector2d(rad * cos(theta), rad * sin(theta));

			// escape velocity v_e
			auto vel_esc = sqrt(2. * G * m_tot) * pow(rad * rad + RAD_PL * RAD_PL, -0.25f);
			// q is the velocity fraction v / v_e
			auto q = 0.0;
			// g is the value of the distribution function g(q) = q^2 (1 - q^2)^3.5
			auto g = 0.1;
			while (g > q * q * pow(1.0f - q * q, 3.5f)) {
				q = get_rand();
				g = 0.1f * get_rand();
			}
			// v = q * v_e
			auto velocity = q * vel_esc;

			// Random distribution for azimuthal angle
			auto theta_v = 2 * PI * get_rand();
			// Convert from spherical polar to Cartesian coordinates
			auto vel = Vector2d(velocity * cos(theta_v), velocity * sin(theta_v));

			return Body2d(pos + pos_offset, vel + vel_offset, mass, true);
		}
	private:
		const double RAD_PL = 0.1 * radius;
		size_t n;
	};
}

