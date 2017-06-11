#include "Display.h"
#include "TrailManager.h"
#include "Types.h"

namespace nbody
{
	TrailManager::TrailManager() : m_first_update(true), m_vtx_array(sf::Lines)
	{
	}

	TrailManager::~TrailManager()
	{
	}

	void TrailManager::update(Vector2d const* state, size_t const num_bodies)
	{
		auto bodies{ reinterpret_cast<ParticleState const*>(state) };

		if (m_first_update)
		{
			// on first runthrough create circular buffers for body coordinates
			m_world_coords.assign(num_bodies, CircularBuffer<Vector2d>{ s_TRAIL_LENGTH });
			m_first_update = false;
		}

		// all trails are redrawn each frame
		m_vtx_array.clear();

		for (size_t i = 0; i < num_bodies; i++)
		{
			// store the current coordinates of each body in a circular buffer
			// when the buffer is full, new coordinates replace old
			// the capacity of the buffer sets the length of the trails
			m_world_coords[i].push_back(bodies[i].pos);

			for (size_t j = 0; j < m_world_coords[i].size() - 1; j++)
			{
				// when there are at least two coordinates stored
				// can draw the trail
				auto& pt{ m_world_coords[i][j + 1] };
				auto& prev_pt{ m_world_coords[i][j] };

				// trail made up of line segments from (n+1)th to nth point
				m_vtx_array.append(sf::Vertex{ { Display::worldToScreenX(pt.x), Display::worldToScreenY(pt.y) } });
				m_vtx_array.append(sf::Vertex{ { Display::worldToScreenX(prev_pt.x), Display::worldToScreenY(prev_pt.y) } });
			}
		}
	}

	void TrailManager::reset()
	{
		m_vtx_array.clear();
		m_first_update = true;
	}

	void TrailManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(m_vtx_array);
	}
}