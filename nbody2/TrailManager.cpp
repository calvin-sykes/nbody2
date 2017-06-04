#include "Display.h"
#include "TrailManager.h"
#include "Types.h"

namespace nbody
{
	TrailManager::TrailManager() : vtx_array(sf::Lines), first_update(true)
	{
	}

	TrailManager::~TrailManager()
	{
	}

	void TrailManager::update(Vector2d const* state, size_t const num_bodies)
	{
		auto bodies = reinterpret_cast<ParticleState const*>(state);
		
		if (this->first_update)
		{
			// on first runthrough create circular buffers for body coordinates
			this->world_coords.assign(num_bodies, CircularBuffer<Vector2d>(TRAIL_LENGTH));
			this->first_update = false;
		}
		
		// all trails are redrawn each frame
		this->vtx_array.clear();
		
		for (size_t i = 0; i < num_bodies; i++)
		{
			// store the current coordinates of each body in a circular buffer
			// when the buffer is full, new coordinates replace old
			// the capacity of the buffer sets the length of the trails
			this->world_coords[i].push_back(bodies[i].pos);

			for (size_t j = 0; j < world_coords[i].size() - 1; j++)
			{
				// when there are at least two coordinates stored
				// can draw the trail
				auto pt = world_coords[i][j + 1];
				auto prev_pt = world_coords[i][j];		
				
				// trail made up of line segments from (n+1)th to nth point
				this->vtx_array.append(sf::Vertex({ Display::worldToScreenX(pt.x), Display::worldToScreenY(pt.y) }));
				this->vtx_array.append(sf::Vertex({ Display::worldToScreenX(prev_pt.x), Display::worldToScreenY(prev_pt.y) }));
			}

		}
	}

	void TrailManager::reset()
	{
		this->vtx_array.clear();
		this->first_update = true;
	}

	void TrailManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(this->vtx_array);
	}
}