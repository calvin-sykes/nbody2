#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <array>
#include <random>

namespace nbody
{
	template<typename T, size_t N>
	class Vector;
	
	using Vector2d = Vector<double, 2>;
	
	namespace priv
	{
		// Calculate the sign of a value
		// Returns -1 if val < 0, +1 if val > 0, and 0 if val == 0 
		template <typename T>
		int sgn(T val)
		{
			return (T(0) < val) - (val < T(0));
		}

		// Calculate the circular velocity at radius r around a mass central_mass
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