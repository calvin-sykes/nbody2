#include "ColourerVelocity.h"
#include "Constants.h"
#include "Types.h"
#include "Vector.h"

#include <algorithm>

namespace nbody
{
	ColourerVelocity::ColourerVelocity() : IColourer(), m_max_vel(0.)
	{
	}

	ColourerVelocity::~ColourerVelocity()
	{
	}

	std::unique_ptr<IColourer> ColourerVelocity::create()
	{
		return std::make_unique<ColourerVelocity>();
	}

	void ColourerVelocity::setup(size_t const offset, size_t const num_bodies, sf::Color const * cols, ParticleState const* state)
	{
		IColourer::setup(offset, num_bodies, cols);

		auto fastest = std::max_element(state, state + num_bodies, [](ParticleState const& a, ParticleState const& b) { return a.vel.mag() < b.vel.mag(); });

		m_max_vel = fastest->vel.mag();
	}

	void ColourerVelocity::applyImpl(ParticleData const * state, ParticleColourState * colour)
	{
		// WIP
		auto v_mag = state->m_state->vel.mag();

		auto phase = std::min(1.0, v_mag / m_max_vel);
		auto w1 = 1 - phase;
		auto w2 = phase;

		auto red = static_cast<sf::Uint8>(w1 * m_cols[0].r + w2 * m_cols[1].r);
		auto green = static_cast<sf::Uint8>(w1 * m_cols[0].g + w2 * m_cols[1].g);
		auto blue = static_cast<sf::Uint8>(w1 * m_cols[0].b + w2 * m_cols[1].b);

		colour->colour = sf::Color{ red, green, blue };
	}
}
