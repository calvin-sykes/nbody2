#ifndef DISTRIBUTOR_PLUMMER_H
#define DISTRIBUTOR_PLUMMER_H

#include "IDistributor.h"

namespace nbody
{
	// forward declaration
	struct BodyGroupProperties;
	struct ParticleData;
	
	class DistributorPlummer : public IDistributor
	{
	public:
		static std::unique_ptr<IDistributor> create();

		DistributorPlummer() : IDistributor() {}

		void createDistribution(ParticleData & bodies, BodyGroupProperties const& props) const override;
	};
}

#endif // DISTRIBUTOR_PLUMMER_H