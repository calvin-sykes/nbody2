#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef _WIN32
#define NBOS_WINDOWS
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

namespace nbody
{
	template <typename T> const T min(const T a, const T b) {
		return (a > b) ? b : a;
	}

	template <typename T> const T max(const T a, const T b) {
		return (a < b) ? b : a;
	}

	namespace Constants
	{
		double constexpr PI = 3.141592;
		double constexpr G = 6.67E-11;
		double constexpr RADIUS = 1E18;
		double constexpr SOFTENING = 0.005 * RADIUS;
		double constexpr SOLAR_MASS = 1.98892E30;
		size_t constexpr MAX_N = 10000;
	}

	// Simulation properties
	size_t constexpr N_MAX = 10000;
	size_t constexpr N = 10000;

	double constexpr TIMESTEP = 1E10;

	double constexpr CENTRAL_MASS = 1E6 * Constants::SOLAR_MASS;
	double constexpr BODY_MASS = 1E1 * Constants::SOLAR_MASS;

	double constexpr BH_THETA = 1;
}

#endif // CONSTANTS_H