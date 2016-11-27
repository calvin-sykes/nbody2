#ifndef CONSTANTS_H
#define CONSTANTS_H
#define _CRT_SECURE_NO_WARNINGS

#include "Vector.h"

#include <iostream>

namespace nbody
{
	// Convenience macros to convert between screen and world coordinates
#define WORLD_TO_SCREEN_X(var) static_cast<float>((var) / RAD * screen_size.x / (screen_scale * aspect_ratio * 2) + 0.5 * screen_size.x - screen_offset.x)

#define WORLD_TO_SCREEN_Y(var) static_cast<float>(-1 * ((var) / RAD) * screen_size.y / (screen_scale * 2) + 0.5 * screen_size.y - screen_offset.y)

#define WORLD_TO_SCREEN_SIZE(var) static_cast<float>((var) / RAD * screen_size.y / (screen_scale * 2))

#define SCREEN_TO_WORLD_X(var) static_cast<float>(((var) - 0.5 * screen_size.x + screen_offset.x) * (2 * aspect_ratio * screen_scale * RAD) / screen_size.x)

#define SCREEN_TO_WORLD_Y(var) static_cast<float>(-1 * ((var) - 0.5* screen_size.y + screen_offset.y) * (2 * screen_scale * RAD) / screen_size.y)

	template <typename T> const T min(const T a, const T b) {
		return (a>b) ? b : a;
	}

	template <typename T> const T max(const T a, const T b) {
		return (a<b) ? b : a;
	}

	// Physical constants
	double constexpr PI = 3.141592;
	double constexpr RAD = 1E18; // simulation radius
	double constexpr M_SOL = 1.98892E30; // solar mass
	double constexpr G = 6.67E-11; // gravitational constant
	double constexpr EPS = 0.005 * RAD; // softening

	// Window properties
	float extern aspect_ratio;
	float extern screen_scale;
	Vector2f extern screen_size;
	Vector2f extern screen_offset;
	//size_t extern screen_width;
	//size_t extern screen_height;

	// Simulation properties
	size_t constexpr N_MAX = 10000;
	size_t constexpr N = 10000;

	double constexpr TIMESTEP = 1E10;

	double constexpr CENTRAL_MASS = 1E6 * M_SOL;
	double constexpr BODY_MASS = 1E1 * M_SOL;

	double constexpr BH_THETA = 1;
}

#endif // CONSTANTS_H