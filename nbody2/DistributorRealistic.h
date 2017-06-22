#ifndef DISTRIBUTOR_REALISTIC_H
#define DISTRIBUTOR_REALISTIC_H

#include "IDistributor.h"

namespace nbody
{
	// forward declaration
	struct BodyGroupProperties;
	struct ParticleData;

	class DistributorRealistic : public IDistributor
	{
	public:
		DistributorRealistic();
		virtual ~DistributorRealistic();

		static std::unique_ptr<IDistributor> create();

		void createDistribution(ParticleData & bodies, BodyGroupProperties const& props) const override;

	private:

		// stuff
	};
}

#endif // DISTRIBUTOR_REALISTIC_H