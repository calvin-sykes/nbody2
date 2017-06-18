#ifndef DISTRIBUTOR_ISOTHERMAL_H
#define DISTRIBUTOR_ISOTHERMAL_H

#include "IDistributor.h"

namespace nbody
{
	// forward declaration
	struct BodyGroupProperties;
	struct ParticleData;
	
	class DistributorIsothermal : public IDistributor
	{
	public:
		static std::unique_ptr<IDistributor> create();

		DistributorIsothermal() : IDistributor() {}

		void createDistribution(ParticleData & bodies, BodyGroupProperties const& props) const override;
	};
}

#endif // DISTRIBUTOR_ISOTHERMAL_H