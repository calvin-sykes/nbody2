#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef> // for size_t under GCC

namespace nbody
{
	/*template <typename T>
	T min(const T a, const T b) {
		return (a > b) ? b : a;
	}

	template <typename T>
	T max(const T a, const T b) {
		return (a < b) ? b : a;
	}*/

	namespace Constants
	{
		double constexpr PI = 3.141592;
		double constexpr G = 6.67E-11;
		double constexpr PARSEC = 3.08567758e16;
		double constexpr SOFTENING = 100 * PARSEC;
		double constexpr SOLAR_MASS = 1.98892E30;
		size_t constexpr MAX_N = 50000;
	}
}

#endif // CONSTANTS_H
