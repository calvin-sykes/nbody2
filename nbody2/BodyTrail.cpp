#include "BodyTrail.h"
#include "Constants.h"
#include "Display.h"

namespace nbody
{
	BodyTrail::BodyTrail(const Vector2d& posIn) : last(0), screen_coords(sf::LineStrip)
	{
		for (size_t i = 0; i < NPOINTS; i++)
		{
			world_coords[i] = posIn;
			screen_coords.append(sf::Vertex({ Display::worldToScreenX(posIn.x), Display::worldToScreenY(posIn.y) }));
		}
	}

	void BodyTrail::update(const Vector2d& pos)
	{
		// if trail has already been cleared, fill world position array with current position
		// to prevent spurious trails from old world positions being drawn 
		if (screen_coords.getVertexCount() == 0)
		{
			for (size_t i = 0; i < NPOINTS; i++)
				world_coords[i] = pos;
		}

		screen_coords.clear();

		for (size_t i = 0; i < NPOINTS - 1; i++)
		{
			// shift forward current world position data
			world_coords[i] = world_coords[i + 1];
			// add the corresponding screen position to the trail
			screen_coords.append(sf::Vertex({ Display::worldToScreenX(world_coords[i].x),
				Display::worldToScreenY(world_coords[i].y) }));
		}
		// slot in new world position data at end of array
		world_coords[NPOINTS - 1] = pos;
		// add the corresponding screen position to the trail
		screen_coords.append(sf::Vertex({ Display::worldToScreenX(pos.x), Display::worldToScreenY(pos.y) }));
	}

	void BodyTrail::reset()
	{
		screen_coords.clear();
	}

	void BodyTrail::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(screen_coords);
	}
}