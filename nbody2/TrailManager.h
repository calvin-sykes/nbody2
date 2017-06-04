#ifndef TRAIL_MANAGER_H
#define TRAIL_MANAGER_H

#include "CircularBuffer.h"
#include "Vector.h"

#include <vector>

#include <SFML/Graphics.hpp>

namespace nbody
{
	constexpr size_t TRAIL_LENGTH = 10;
	
	class Body2d;
	class TrailManager : public sf::Drawable
	{
	public:
		TrailManager();
		~TrailManager();

		void update(Vector2d const* state, size_t const num_bodies);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void reset();

	private:
		bool first_update;

		std::vector<CircularBuffer<Vector2d>> world_coords;
		sf::VertexArray vtx_array;
	};
}

#endif // TRAIL_MANAGER_H