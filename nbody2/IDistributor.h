#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <array>
//#include <memory>
#include <random>

namespace nbody
{
	template<typename T, size_t N>
	class Vector;
	
	using Vector2d = Vector<double, 2>;
	
	namespace priv
	{

		/**
		 * \brief Calculate the sign of a value
		 * \tparam T The type of the value being tested
		 * \param val The value to test
		 * \return -1 if val < 0, +1 if val > 0, and 0 if val == 0
		 */
		template <typename T>
		int sgn(T val)
		{
			static_assert(std::is_arithmetic<T>::value, "Cannot calculate sign of non-numerical value");
			return (T(0) < val) - (val < T(0));
		}

		// 
		/**
		 * \brief Calculate the velocity needed for a circular orbit
		 * \param r The radius of the orbit
		 * \param central_mass The mass being orbited
		 * \return 
		 */
		Vector2d vCirc(Vector2d const& r, double const central_mass);
	}

	// forward declaration
	struct BodyGroupProperties;
	struct ParticleData;

	using param_t = std::uniform_real_distribution<>::param_type;

	enum class DistributorType
	{
		EXPONENTIAL,
		ISOTHERMAL,
		PLUMMER,
		REALISTIC,
		N_DISTRIBUTIONS,
		INVALID = -1
	};

	struct DistributorProperties
	{
		constexpr DistributorProperties(DistributorType const type, char const* name, char const* tooltip, bool const has_central_mass)
			: type(type),
			  name(name),
			  tooltip(tooltip),
			  has_central_mass(has_central_mass)
		{
		}

		DistributorType const type;
		char const* name;
		char const* tooltip;
		bool const has_central_mass;
	};

	using DPArray = std::array<DistributorProperties, static_cast<size_t>(DistributorType::N_DISTRIBUTIONS)>;

	constexpr DPArray m_dist_infos = { {
		{
			DistributorType::EXPONENTIAL,
			"Exponential",
			"The density of bodies falls off exponentially with distance from a large central mass.",
			true
		},
		{
			DistributorType::ISOTHERMAL,
			"Isothermal",
			"The density of bodies falls off with the square of the distance from a large central mass.",
			true
		},
		{
			DistributorType::PLUMMER,
			"Plummer",
			"Bodies are distributed according to the Plummer globular cluster model.",
			false
		},
		{
			DistributorType::REALISTIC,
			"Realistic",
			"Bodies are distributed using the density-wave theory for spiral arms.",
			true
		}
		} };

	class IDistributor
	{
	public:
		IDistributor()
		{
			getRand = [](double lower, double upper)
			{
				return m_dist(m_gen, param_t{ lower, upper });
			};
		}

		virtual ~IDistributor() = default;

		virtual void createDistribution(ParticleData & bodies, BodyGroupProperties const&) const = 0;

	protected:
		double (*getRand)(double, double);
		static std::default_random_engine m_gen;
		static std::uniform_real_distribution<> m_dist;
	};
}

#endif // DISTRIBUTOR_H