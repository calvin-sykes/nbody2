#include "BodyManager.h"
#include "Display.h"
#include "Types.h"

namespace nbody
{
	BodyManager::BodyManager() : m_vtx_array(sf::Triangles), m_scl(0), m_first_update(true)
	{
		// cache the vertices of a unit circle
		for (size_t i = 0; i < s_VERTICES; i++)
		{
			m_unit_circle[i] = { cos(i * s_THETA), sin(i * s_THETA) };
		}
	}

	BodyManager::~BodyManager()
	{
	}

	void BodyManager::update(Vector2d const* state, ParticleAuxState const* aux_state, ParticleColourState const* colour_state, size_t const num_bodies)
	{
		auto bodies = reinterpret_cast<ParticleState const*>(state);

		if (m_first_update)
		{
			// on first step, cache all (screen-coordinate) radii
			m_radii.resize(num_bodies);
			for (size_t i = 0; i < num_bodies; i++)
			{
				m_radii[i] = radiusFromMass(aux_state[i].mass);
			}
			m_first_update = false;
		}

		m_scl = Display::bodyScalingFunc(Display::screen_scale);

		m_vtx_array.clear();

		for (size_t i = 0; i < num_bodies; i++)
		{
			drawBody(bodies[i], colour_state[i], i);
		}
	}

	void BodyManager::drawBody(ParticleState const & p, ParticleColourState const& c, size_t const idx)
	{
		auto world_pos = p.pos;

		auto screen_x = Display::worldToScreenX(world_pos.x);
		auto screen_y = Display::worldToScreenY(world_pos.y);

		auto is_visible = screen_x < Display::screen_size.x && screen_x > 0 && screen_y < Display::screen_size.y && screen_y > 0;

		if (is_visible)
		{
			auto pos = sf::Vector2f{ screen_x, screen_y };
			auto radius = m_radii[idx] * m_scl;

			auto col = c.colour;
			auto count = radius > 5 ? s_VERTICES : s_VERTICES_SMALL;

			// Calculate positions of vertices around the edge of the body
			for (size_t i = 0; i < count; i++)
			{
				m_scratch[i] = m_unit_circle[radius > 5 ? i : i * 2] * radius;
				m_scratch[i] += pos;
			}

			// put into vertex array
			for (size_t i = 0; i < count - 1; i++)
			{
				m_vtx_array.append({ pos, col });
				m_vtx_array.append({ m_scratch[i], col });
				m_vtx_array.append({ m_scratch[i + 1], col });
			}
			// last point reconnects to first
			m_vtx_array.append({ pos, col });
			m_vtx_array.append({ m_scratch[count - 1], col });
			m_vtx_array.append({ m_scratch[0], col });
		}
	}

	void BodyManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(m_vtx_array);
	}

	void BodyManager::setDirty()
	{
		m_first_update = true;
	}

	float BodyManager::radiusFromMass(double mass) const
	{
		if (mass < Constants::SOLAR_MASS * 1e5)
			return std::max(float(s_MIN_SIZE), std::min(static_cast<float>(1 + log10(mass / Constants::SOLAR_MASS)), float(s_MAX_SIZE)));
		
		// special case for black holes
			return s_BH_SIZE;
	}
}