#include "BodyGroupProperties.h"
#include "Constants.h"
#include "DistributorPlummer.h"

namespace nbody
{
	std::unique_ptr<IDistributor> DistributorPlummer::create()
	{
		return std::make_unique<DistributorPlummer>();
	}
	
	void DistributorPlummer::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
	{
		auto pos_offset = props.pos;
		auto vel_offset = props.vel;
		auto rad = props.radius;
		if (props.use_parsecs)
		{
			pos_offset *= Constants::PARSEC;
			rad *= Constants::PARSEC;
		}
		auto plummer_rad = 0.1 * rad;
		auto m_avg = 0.5 * (props.min_mass + props.max_mass) * Constants::SOLAR_MASS;
		auto m_tot = props.num * m_avg;

		for (size_t i = 0; i < props.num; i++)
		{
			auto mass = getRand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			// Calculate radius as function of total mass enclosed inside that radius
			// which is chosen randomly
			auto fractional_rad = plummer_rad / sqrt(pow(getRand(0, 1), -2.0 / 3.0) - 1);
			// Random distribution for azimuthal angle
			auto phi = getRand(0, 2 * Constants::PI);
			auto pos = Vector2d{ fractional_rad * cos(phi), fractional_rad * sin(phi) };

			// escape velocity v_e
			auto vel_esc = sqrt(2. * Constants::G * m_tot) * pow(fractional_rad * fractional_rad + plummer_rad * plummer_rad, -0.25);
			// q is the velocity fraction v / v_e
			auto q = 0.0;
			// g is the value of the distribution function g(q) = q^2 (1 - q^2)^3.5
			auto g = 0.1;
			while (g > q * q * pow(1.0 - q * q, 3.5)) {
				q = getRand(0, 1);
				g = getRand(0, 0.1);
			}
			// v = q * v_e
			auto velocity = q * vel_esc;
			// Random distribution for azimuthal velocity angle
			auto phi_v = getRand(0, 2 * Constants::PI);
			// Convert from spherical polar to Cartesian coordinates
			auto vel = Vector2d{ velocity * cos(phi_v), velocity * sin(phi_v) };
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
	}
}