#include "BodyDistributor.h"
#include "Body2d.h"
#include "BodyGroupProperties.h"
#include "Constants.h"
#include "Vector.h"

namespace nbody
{
	namespace priv
	{
		Vector2d vCirc(Vector2d const & r, double const central_mass)
		{
			auto mag_v = sqrt(Constants::G * central_mass / r.mag());
			auto angle_v = atan(abs(r.y / r.x));
			auto v_x = -1 * sgn(r.y) * sin(angle_v) * mag_v;
			auto v_y = sgn(r.x) * cos(angle_v) * mag_v;
			return Vector2d(v_x, v_y);
		}
	}

	using namespace priv;

	std::vector<Body2d> nbody::ExponentialDistributor::createDistribution(BodyGroupProperties const & props) const
	{
		auto pos_offset = props.pos;
		auto vel_offset = props.vel;
		auto rad = props.radius;
		if (props.use_relative_coords)
		{
			pos_offset *= Constants::RADIUS;
			vel_offset *= Constants::RADIUS;
			rad *= Constants::RADIUS;
		}
		std::vector<Body2d> bodies;
		bodies.reserve(props.N);
		bodies.emplace_back(Body2d(pos_offset, vel_offset, props.central_mass * 1E6 * Constants::SOLAR_MASS));
		for (size_t i = 1; i < props.N; i++)
		{
			auto mass = get_rand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			auto radius = rad * expDist(lambda) + Constants::SOFTENING;
			auto phi = get_rand(0, 2 * Constants::PI);
			Vector2d pos(radius * cos(phi), radius * sin(phi));
			Vector2d vel(vCirc(pos, props.central_mass * 1E6 * Constants::SOLAR_MASS));
			bodies.emplace_back(Body2d(pos + pos_offset, vel + vel_offset, mass));
		}
		return bodies;
	}

	std::vector<Body2d> IsothermalDistributor::createDistribution(BodyGroupProperties const & props) const
	{
		auto pos_offset = props.pos;
		auto vel_offset = props.vel;
		auto rad = props.radius;
		if (props.use_relative_coords)
		{
			pos_offset *= Constants::RADIUS;
			vel_offset *= Constants::RADIUS;
			rad *= Constants::RADIUS;
		}
		std::vector<Body2d> bodies;
		bodies.reserve(props.N);
		bodies.emplace_back(Body2d(pos_offset, vel_offset, props.central_mass * 1E6 * Constants::SOLAR_MASS));
		for (size_t i = 1; i < props.N; i++)
		{
			auto mass = get_rand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			auto radius = get_rand(0, rad) + Constants::SOFTENING;
			auto phi = get_rand(0, 2 * Constants::PI);
			Vector2d pos(radius * cos(phi), radius * sin(phi));
			Vector2d vel(vCirc(pos, props.central_mass * 1E6 * Constants::SOLAR_MASS));
			bodies.emplace_back(Body2d(pos + pos_offset, vel + vel_offset, mass));
		}
		return bodies;
	}

	std::vector<Body2d> PlummerDistributor::createDistribution(BodyGroupProperties const & props) const
	{
		auto pos_offset = props.pos;
		auto vel_offset = props.vel;
		auto rad = props.radius;
		if (props.use_relative_coords)
		{
			pos_offset *= Constants::RADIUS;
			vel_offset *= Constants::RADIUS;
			rad *= Constants::RADIUS;
		}
		auto plummer_rad = 0.1 * rad;
		auto m_avg = 0.5 * (props.min_mass + props.max_mass);
		auto m_tot = props.N * m_avg;
		std::vector<Body2d> bodies;
		bodies.reserve(props.N);
		for (size_t i = 0; i < props.N; i++)
		{
			auto mass = get_rand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			// Calculate radius as function of total mass enclosed inside that radius
			// which is chosen randomly
			auto fractional_rad = plummer_rad / sqrt(pow(get_rand(0, 1), -2.0 / 3.0) - 1);
			// Random distribution for azimuthal angle
			auto phi = get_rand(0, 2 * Constants::PI);
			Vector2d pos(rad * cos(phi), rad * sin(phi));

			// escape velocity v_e
			auto vel_esc = sqrt(2. * Constants::G * m_tot) * pow(rad * rad + plummer_rad * plummer_rad, -0.25);
			// q is the velocity fraction v / v_e
			auto q = 0.0;
			// g is the value of the distribution function g(q) = q^2 (1 - q^2)^3.5
			auto g = 0.1;
			while (g > q * q * pow(1.0 - q * q, 3.5)) {
				q = get_rand(0, 1);
				g = get_rand(0, 0.1);
			}
			// v = q * v_e
			auto velocity = q * vel_esc;
			// Random distribution for azimuthal velocity angle
			auto phi_v = get_rand(0, 2 * Constants::PI);
			// Convert from spherical polar to Cartesian coordinates
			Vector2d vel(velocity * cos(phi_v), velocity * sin(phi_v));
			bodies.emplace_back(Body2d(pos + pos_offset, vel + vel_offset, mass));
		}
		return bodies;
	}
}