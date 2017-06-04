#include "BodyManager.h"
#include "Display.h"
#include "Types.h"

namespace nbody
{
	BodyManager::BodyManager() : vtx_array(sf::Triangles), first_update(true)
	{
		// cache the vertices of a unit circle
		for (size_t i = 0; i < this->N_VERTICES; i++)
		{
			this->UNIT_CIRCLE[i] = { cos(i * this->THETA), sin(i * this->THETA) };
		}
	}

	BodyManager::~BodyManager()
	{
	}

	void nbody::BodyManager::update(Vector2d const* state, ParticleAuxState const* aux_state, size_t const num_bodies)
	{
		auto bodies = reinterpret_cast<ParticleState const*>(state);
		
		if (this->first_update == true)
		{
			// on first step, cache all (screen-coordinate) radii
			this->radii.resize(num_bodies);
			for (size_t i = 0; i < num_bodies; i++)
			{
				this->radii[i] = this->radiusFromMass(aux_state[i].mass);
			}
			this->first_update = false;
		}

		this->scl = max(static_cast<float>(-std::log2(Display::screen_scale)), 1.f);

		this->vtx_array.clear();

		for (size_t i = 0; i < num_bodies; i++)
		{
			this->drawBody(bodies[i], i);
		}
	}

	void BodyManager::drawBody(ParticleState const & p, size_t const idx)
	{
		auto world_pos = p.pos;
		
		auto screen_x = Display::worldToScreenX(world_pos.x);
		auto screen_y = Display::worldToScreenY(world_pos.y);

		auto is_visible = screen_x < Display::screen_size.x && screen_x > 0 && screen_y < Display::screen_size.y && screen_y > 0;
		
		if (is_visible)
		{
			auto pos = sf::Vector2f{ screen_x, screen_y };
			auto radius = this->radii[idx] * this->scl;

			// WIP
			auto v_mag = p.vel.mag();
			auto phase = min(Constants::PI / 2., (v_mag * 1e-5) * (Constants::PI / 2.));
			auto red = (int)(254 * sin(phase));
			auto blue = (int)(254 * cos(phase));
			auto green = 0;
			auto col = sf::Color(red, green, blue);

			// Calculate positions of vertices around the edge of the body
			for (size_t i = 0; i < this->N_VERTICES; i++)
			{
				this->scratch[i] = UNIT_CIRCLE[i] * radius;
				this->scratch[i] += pos;
			}

			// put into vertex array
			for (size_t i = 0; i < this->N_VERTICES - 1; i++)
			{
				vtx_array.append({ pos, col });
				vtx_array.append({ scratch[i], col });
				vtx_array.append({ scratch[i + 1], col });
			}
			// last point reconnects to first
			vtx_array.append({ pos, col });
			vtx_array.append({ scratch[this->N_VERTICES - 1], col });
			vtx_array.append({ scratch[0], col });
		}
	}

	void BodyManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(vtx_array);
	}

	float BodyManager::radiusFromMass(double mass)
	{
		return min(static_cast<float>(MIN_SIZE * log10(mass / Constants::SOLAR_MASS)), MAX_SIZE);
	}
}