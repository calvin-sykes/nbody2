#include "BodyGroupProperties.h"
#include "Constants.h"
#include "DistributorRealistic.h"

namespace nbody
{
	DistributorRealistic::DistributorRealistic()
	{
	}

	DistributorRealistic::~DistributorRealistic()
	{
	}

	std::unique_ptr<IDistributor> DistributorRealistic::create()
	{
		return std::make_unique<DistributorRealistic>();
	}
	void DistributorRealistic::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
	{
		auto pos_offset = props.pos;
		auto vel_offset = props.vel;
		auto rad = props.radius;
		auto core_rad = rad * 0.3;
		if (props.use_relative_coords)
		{
			pos_offset *= Constants::RADIUS;
			vel_offset *= Constants::RADIUS;
			rad *= Constants::RADIUS;
		}

		constexpr auto alpha = 2.35;
		auto k = (1 - alpha) / (pow(props.max_mass, 1 - alpha) - pow(props.min_mass, 1 - alpha));

		// central mass
		bodies.m_state[0].pos = pos_offset;
		bodies.m_state[0].vel = vel_offset;
		bodies.m_aux_state[0].mass = props.central_mass * Constants::SOLAR_MASS;

		for (size_t i = 1; i < props.num; i++)
		{
			auto mass = salpeterIMF(alpha, k, props.min_mass) * Constants::SOLAR_MASS;
			auto radius = getRand(0, rad) + Constants::SOFTENING;
			auto phi = getRand(0, 2 * Constants::PI);
			auto pos = Vector2d{ radius * cos(phi), radius * sin(phi) };
			auto vel = vCirc(pos, props.central_mass * Constants::SOLAR_MASS);
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
	}

	double DistributorRealistic::salpeterIMF(double const alpha, double const k, double const lb) const
	{
		auto x = getRand(0, 1);
		return pow(x * (1 - alpha) / k + pow(lb, 1 - alpha), 1 / (1 - alpha));
	}
}
