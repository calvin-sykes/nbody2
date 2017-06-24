#ifndef TIMINGS_H
#define TIMINGS_H

#include <chrono>
#include <map>

namespace nbody
{
	using Clock = std::chrono::steady_clock;
	using Dble_ms = std::chrono::duration<double, std::ratio<1, 1000>>;

	enum class Timings
	{
		RUN_START,
		TREE_BUILD_START,
		TREE_BUILD_END,
		FORCE_CALC_START,
		FORCE_CALC_END,
		DRAW_BODIES_START,
		DRAW_BODIES_END,
		DRAW_GRID_START,
		DRAW_GRID_END,
		DRAW_TRAILS_START,
		DRAW_TRAILS_END,
		ENERGY_CALC_START,
		ENERGY_CALC_END
	};

	extern std::map<Timings, std::chrono::time_point<Clock>> timings;
}

#endif // TIMINGS_H