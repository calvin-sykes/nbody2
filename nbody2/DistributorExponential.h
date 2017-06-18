#ifndef DISTRIBUTOR_EXPONENTIAL_H
#define DISTRIBUTOR_EXPONENTIAL_H

#include "IDistributor.h"

namespace nbody
{
	// forward declaration
	struct BodyGroupProperties;
	struct ParticleData;
	
	class DistributorExponential : public IDistributor
	{
	public:
		static std::unique_ptr<IDistributor> create();

		DistributorExponential() : IDistributor()
		{
			getExpRand = []()
			{
				return m_exp_dist(m_gen);
			};
		}

		void createDistribution(ParticleData & bodies, BodyGroupProperties const& props) const override;

	private:
		static std::exponential_distribution<> m_exp_dist;
		double(*getExpRand)();

		double static constexpr s_lambda = 4.0;
	};
}

#endif // DISTRIBUTOR_EXPONENTIAL_H
