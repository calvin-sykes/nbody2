#ifndef TRAIL_MANAGER_H
#define TRAIL_MANAGER_H

#include "CircularBuffer.h"
#include "Vector.h"

#include <vector>

#include <SFML/Graphics.hpp>

namespace nbody
{	
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
		constexpr static size_t s_TRAIL_LENGTH = 10;

		bool m_first_update;

		std::vector<CircularBuffer<Vector2d>> m_world_coords;
		sf::VertexArray m_vtx_array;
	};
}

#endif // TRAIL_MANAGER_H