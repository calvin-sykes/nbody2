#include "BodyDistributor.h"
#include "BodyGroupProperties.h"
#include "Constants.h"
#include "Types.h"
#include "Vector.h"

#include <chrono>

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
			return{ v_x, v_y };
		}
	}

	using namespace priv;

	std::default_random_engine BodyDistributor::gen =
		std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
	std::uniform_real_distribution<> BodyDistributor::dist = std::uniform_real_distribution<>(0, 1);

	std::unique_ptr<BodyDistributor> ExponentialDistributor::create()
	{
		return std::make_unique<ExponentialDistributor>();
	}

	void nbody::ExponentialDistributor::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
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

		// central mass
		bodies.m_state[0].pos = pos_offset;
		bodies.m_state[0].vel = vel_offset;
		bodies.m_aux_state[0].mass = props.central_mass * Constants::SOLAR_MASS;

		for (size_t i = 1; i < props.num; i++)
		{
			auto mass = get_rand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			auto radius = rad * expDist(lambda) + Constants::SOFTENING;
			auto phi = get_rand(0, 2 * Constants::PI);
			auto pos = Vector2d{ radius * cos(phi), radius * sin(phi) };
			auto vel = vCirc(pos, props.central_mass * Constants::SOLAR_MASS);
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
		return;
	}

	std::unique_ptr<BodyDistributor> IsothermalDistributor::create()
	{
		return std::make_unique<IsothermalDistributor>();
	}

	void IsothermalDistributor::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
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

		// central mass
		bodies.m_state[0].pos = pos_offset;
		bodies.m_state[0].vel = vel_offset;
		bodies.m_aux_state[0].mass = props.central_mass * Constants::SOLAR_MASS;

		for (size_t i = 1; i < props.num; i++)
		{
			auto mass = get_rand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			auto radius = get_rand(0, rad) + Constants::SOFTENING;
			auto phi = get_rand(0, 2 * Constants::PI);
			auto pos = Vector2d{ radius * cos(phi), radius * sin(phi) };
			auto vel = vCirc(pos, props.central_mass * Constants::SOLAR_MASS);
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
		return;
	}

	std::unique_ptr<BodyDistributor> PlummerDistributor::create()
	{
		return std::make_unique<PlummerDistributor>();
	}

	void PlummerDistributor::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
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
		auto m_avg = 0.5 * (props.min_mass + props.max_mass) * Constants::SOLAR_MASS;
		auto m_tot = props.num * m_avg;

		for (size_t i = 0; i < props.num; i++)
		{
			auto mass = get_rand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			// Calculate radius as function of total mass enclosed inside that radius
			// which is chosen randomly
			auto fractional_rad = plummer_rad / sqrt(pow(get_rand(0, 1), -2.0 / 3.0) - 1);
			// Random distribution for azimuthal angle
			auto phi = get_rand(0, 2 * Constants::PI);
			auto pos = Vector2d{ fractional_rad * cos(phi), fractional_rad * sin(phi) };

			// escape velocity v_e
			auto vel_esc = sqrt(2. * Constants::G * m_tot) * pow(fractional_rad * fractional_rad + plummer_rad * plummer_rad, -0.25);
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
			auto vel = Vector2d{ velocity * cos(phi_v), velocity * sin(phi_v) };
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
		return;
	}
}