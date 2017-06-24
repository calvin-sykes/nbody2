#ifndef DISTRIBUTOR_REALISTIC_H
#define DISTRIBUTOR_REALISTIC_H

#include "ComplexCDF.h"
#include "IDistributor.h"

namespace nbody
{
	// forward declaration
	struct BodyGroupProperties;
	struct ParticleData;

	class DistributorRealistic : public IDistributor
	{
		friend class RunState;

	public:
		DistributorRealistic();
		virtual ~DistributorRealistic();

		static std::unique_ptr<IDistributor> create();

		void createDistribution(ParticleData & bodies, BodyGroupProperties const& props) const override;

	private:
		/**
		* \brief Computes a random mass using the Salpeter initial mass function
		* \param alpha Slope of the power law
		* \param k Normalisation constant
		* \param lb Minimum mass value (should be >= 0.5Msun)
		* \return The computed mass value
		*/
		double salpeterIMF(double const alpha, double const k, double const lb) const;
		
		
		/**
		 * \brief Determine the eccentricity of an orbit with specified radius
		 * \param rad The radius at which to calculate the eccentricity
		 * \param core_rad The radius of the cpherical core of the galaxy
		 * \param galaxy_rad The outer radius of the galaxy
		 * \return The computed eccentricity value
		 */
		static double eccentricity(double const rad, double const core_rad, double const galaxy_rad);
		
		
		static Vector2d velocity(Vector2d const& pos, double const a, double const m, double const e, double const phi);

		mutable ComplexCDF m_cdf;

		static int nl, nr;

		constexpr static double s_ECC_CORE = 0.35;
		constexpr static double s_ECC_DISK = 0.2;
		constexpr static int s_NUM_PERTS = 2;
		constexpr static double s_PERT_DAMP = 40;
	};
}

#endif // DISTRIBUTOR_REALISTIC_H