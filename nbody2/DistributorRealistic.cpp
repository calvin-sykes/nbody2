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
		auto core_rad = props.radius * 0.3;
		if (props.use_relative_coords)
		{
			pos_offset *= Constants::RADIUS;
			vel_offset *= Constants::RADIUS;
			rad *= Constants::RADIUS;
		}
	}
}
