#include "BodyGroupProperties.h"
#include "Constants.h"
#include "DistributorRealistic.h"

namespace nbody
{
	std::uniform_int_distribution<> DistributorRealistic::m_int_dist{};

	DistributorRealistic::DistributorRealistic()
	{
		getIntRand = [](int lower, int upper)
		{
			return m_int_dist(m_gen, std::uniform_int_distribution<>::param_type{ lower, upper });
		};
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
		auto const pos_offset = props.pos;
		auto const vel_offset = props.vel;
		auto const galaxy_rad = props.radius;
		auto const core_rad = galaxy_rad * 0.3;
		auto const delta_ang = 2 * Constants::PI / galaxy_rad;

		m_cdf.setup(1.0, 0.02, galaxy_rad / 3.0, core_rad, 0, galaxy_rad * 2, 1000);

		// Initial mass function
		constexpr auto pow_alpha = 2.35;

		// central mass
		bodies.m_state[0].pos = pos_offset;
		bodies.m_state[0].vel = vel_offset;
		bodies.m_aux_state[0].mass = props.central_mass * Constants::SOLAR_MASS;

		// random distribution properties
		auto const n_arms = getIntRand(2, 4);
		auto const arm_sep = 2 * Constants::PI / n_arms;
		auto const angle_offset = getRand(0, 2 * Constants::PI);

		if (props.use_parsecs)
		{
			bodies.m_state[0].pos *= Constants::PARSEC;
		}

		for (auto i = 1; i < props.num; i++)
		{
			auto picked_rad = m_cdf.valFromProbability(getRand(0, 1)) + 10 * Constants::SOFTENING / Constants::PARSEC;
			auto a = picked_rad;
			auto e = eccentricity(a, core_rad, galaxy_rad);

			auto angle = a * delta_ang;
			auto alpha = getRand(0, 2 * Constants::PI);


			auto beta = angle + angle_offset + arm_sep * getIntRand(0, n_arms - 1);

			auto r = a * (1 - e * e) / (1 + e * cos(alpha - beta));

			// smudging
			alpha += getRand(-0.05, 0.05);

			auto pos = Vector2d{ r * cos(alpha), r * sin(alpha) };

			auto vel = velocity(pos * Constants::PARSEC,
				a * Constants::PARSEC,
				props.central_mass * Constants::SOLAR_MASS,
				e,
				beta);

			auto mass_lims = constrainMasses({ props.min_mass, props.max_mass }, picked_rad, core_rad, galaxy_rad);
			auto k = (1 - pow_alpha) / (pow(mass_lims.second, 1 - pow_alpha) - pow(mass_lims.first, 1 - pow_alpha));
			auto mass = salpeterIMF(pow_alpha, k, mass_lims.first);

			bodies.m_state[i].pos = pos + pos_offset;
			bodies.m_state[i].vel = vel + vel_offset;
			bodies.m_aux_state[i].mass = mass  * Constants::SOLAR_MASS;

			if (props.use_parsecs)
			{
				bodies.m_state[i].pos *= Constants::PARSEC;
			}
		}
	}

	std::pair<double, double> DistributorRealistic::constrainMasses(std::pair<double, double> const limits, double const rad, double const core_rad, double const galaxy_rad)
	{
		auto constexpr core_max = 2.0;
		auto constexpr gal_min = 1.0;

		if (rad < core_rad)
			return std::make_pair(limits.first, core_max + (limits.second - core_max) * rad / core_rad);

		if (rad < galaxy_rad)
			return std::make_pair(gal_min, limits.second + (gal_min - limits.second) / (galaxy_rad - core_rad) * (rad - core_rad));

		return std::make_pair(limits.first, 2.0);
	}

	double DistributorRealistic::salpeterIMF(double const alpha, double const k, double const lb) const
	{
		auto x = getRand(0, 1);
		return pow(x * (1 - alpha) / k + pow(lb, 1 - alpha), 1 / (1 - alpha));
	}

	double DistributorRealistic::eccentricity(double const rad, double const core_rad, double const galaxy_rad)
	{
		if (rad < core_rad)
			return rad / core_rad * s_ECC_CORE;

		if (rad < galaxy_rad)
			return s_ECC_CORE + (rad - core_rad) / (galaxy_rad - core_rad) * (s_ECC_DISK - s_ECC_CORE);

		if (rad < 2 * galaxy_rad)
			return s_ECC_DISK + (rad - galaxy_rad) / galaxy_rad * (0.0 - s_ECC_DISK);

		return 0;
	}

	Vector2d DistributorRealistic::velocity(Vector2d const & pos, double const a, double const m, double const e, double const phi)
	{
		auto p = 2.0 * Constants::PI / sqrt(Constants::G * m) * pow(a, 3.0 / 2.0);
		auto theta = atan2(pos.y, pos.x);

		auto vr = 2.0 * Constants::PI * a * e * sin(theta - phi) / (p * sqrt(1 - e * e));
		auto vtheta = 2.0 * Constants::PI * a * (1.0 + e * cos(theta - phi)) / (p * sqrt(1 - e * e));

		auto vx = vr * cos(theta) - vtheta * sin(theta);
		auto vy = vr * sin(theta) + vtheta * cos(theta);

		return { vx, vy };
	}
}