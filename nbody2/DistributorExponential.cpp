#include "BodyGroupProperties.h"
#include "Constants.h"
#include "DistributorExponential.h"

namespace nbody
{
	std::exponential_distribution<> DistributorExponential::m_exp_dist{ s_lambda };

	DistributorExponential::DistributorExponential() : IDistributor()
	{
		getExpRand = []()
		{
			return m_exp_dist(m_gen);
		};
	}

	DistributorExponential::~DistributorExponential()
	{
	}

	std::unique_ptr<IDistributor> DistributorExponential::create()
	{
		return std::make_unique<DistributorExponential>();
	}

	void DistributorExponential::createDistribution(ParticleData & bodies, BodyGroupProperties const & props) const
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
			auto mass = getRand(props.min_mass, props.max_mass) * Constants::SOLAR_MASS;
			auto radius = rad * getExpRand() + Constants::SOFTENING;
			auto phi = getRand(0, 2 * Constants::PI);
			auto pos = Vector2d{ radius * cos(phi), radius * sin(phi) };
			auto vel = vCirc(pos, props.central_mass * Constants::SOLAR_MASS);
			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass;
		}
	}
}