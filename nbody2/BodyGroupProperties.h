#ifndef BODY_GROUP_PROPERTIES_H
#define BODY_GROUP_PROPERTIES_H

#include "BodyDistributor.h"
#include "IColourer.h"
#include "Vector.h"

#include <SFML/Graphics/Color.hpp>

namespace nbody
{
	struct BodyGroupProperties
	{
		BodyGroupProperties() :
			dist(DistributorType::INVALID), num(0), pos(), vel(), radius(0), use_relative_coords(true),
			min_mass(0), max_mass(0), has_central_mass(false), central_mass(0),
			colour(ColourerType::INVALID), cols{} {}

		DistributorType dist;
		int num;
		Vector2d pos;
		Vector2d vel;
		double radius;
		bool use_relative_coords;
		double min_mass;
		double max_mass;
		bool has_central_mass;
		double central_mass;
		ColourerType colour;
		sf::Color cols[MAX_COLS_PER_COLOURER];
	};
}

#endif // BODY_GROUP_PROPERTIES_H