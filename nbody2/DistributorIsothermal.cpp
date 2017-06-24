#include "BodyGroupProperties.h"
#include "Constants.h"
#include "DistributorIsothermal.h"

namespace nbody
{
	std::unique_ptr<IDistributor> DistributorIsothermal::create()
	{
		return std::make_unique<DistributorIsothermal>();
	}

	void DistributorIsothermal::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
	{
		auto pos_offset = props.pos;
		auto vel_offset = props.vel;
		auto rad = props.radius;
		if (props.use_parsecs)
		{
			pos_offset *= Constants::PARSEC;
			rad *= Constants::PARSEC;
		}

		// central mass
		bodies.m_state[0].pos = pos_offset;
		bodies.m_state[0].vel = vel_offset;
		bodies.m_aux_state[0].mass = props.central_mass * Constants::SOLAR_MASS;

		for (size_t i = 1; i < props.num; i++)
		{
			auto mass = getRand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			auto radius = getRand(0, rad) + Constants::SOFTENING;
			auto phi = getRand(0, 2 * Constants::PI);
			auto pos = Vector2d{ radius * cos(phi), radius * sin(phi) };
			auto vel = vCirc(pos, props.central_mass * Constants::SOLAR_MASS);
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
	}
}