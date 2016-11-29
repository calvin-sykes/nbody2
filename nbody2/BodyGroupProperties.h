#ifndef BODY_GROUP_PROPERTIES_H
#define BODY_GROUP_PROPERTIES_H

#include "BodyDistributor.h"
// TODO #include "BodyColourer.h"

namespace nbody
{
	// TODO: Move me to Colourer class when it's been written
	enum class ColourerType
	{
		SOLID,
		VELOCITY,
		N_TYPES,
		INVALID = -1
	};
	// also TODO
	struct ColourerProperties
	{
		ColourerType const type;
		char const* name;
		char const* tooltip;
		char cols_used;
	};
	// also TODO
	size_t constexpr MAX_COLS_PER_COLOURER = 2;
	// also TODO
	struct TempColArray
	{
		float cols[static_cast<size_t>(ColourerType::N_TYPES)][MAX_COLS_PER_COLOURER][3];
	};
	
	struct BodyGroupProperties
	{
		BodyGroupProperties() :
			dist(DistributorType::INVALID), N(0), pos(), vel(),
			min_mass(0), max_mass(0), has_central_mass(false), central_mass(0),
			colour(ColourerType::INVALID), cols{}, ncols(0) {}

		int N;
		DistributorType dist;
		double min_mass;
		double max_mass;
		bool has_central_mass;
		double central_mass;
		Vector2d pos, vel;
		ColourerType colour;
		sf::Color cols[MAX_COLS_PER_COLOURER];
		size_t ncols;
	};
}

#endif // BODY_GROUP_PROPERTIES_H