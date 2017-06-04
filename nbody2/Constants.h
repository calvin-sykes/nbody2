#ifndef CONSTANTS_H
#define CONSTANTS_H

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
		double constexpr SOFTENING = 0.001 * RADIUS;
		double constexpr SOLAR_MASS = 1.98892E30;
		size_t constexpr MAX_N = 20000;
	}

	double constexpr BH_THETA = 1;
}

#endif // CONSTANTS_H