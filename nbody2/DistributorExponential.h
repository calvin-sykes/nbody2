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
		DistributorExponential()
		{
			getExpRand = []()
			{
				return m_exp_dist(m_gen);
			};
		}

		virtual ~DistributorExponential();
		
		static std::unique_ptr<IDistributor> create();

		void createDistribution(ParticleData & bodies, BodyGroupProperties const& props) const override;

	private:
		static std::exponential_distribution<> m_exp_dist;
		double(*getExpRand)();

		double static constexpr s_LAMBDA = 4.0;
	};
}

#endif // DISTRIBUTOR_EXPONENTIAL_H
