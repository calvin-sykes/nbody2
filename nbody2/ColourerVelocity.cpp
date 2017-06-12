#include "Constants.h"
#include "ColourerVelocity.h"
#include "Types.h"

namespace nbody
{
	ColourerVelocity::ColourerVelocity() : IColourer()
	{
	}

	ColourerVelocity::~ColourerVelocity()
	{
	}

	std::unique_ptr<IColourer> ColourerVelocity::create()
	{
		return std::make_unique<ColourerVelocity>();
	}

	void ColourerVelocity::applyImpl(ParticleData const * state, ParticleColourState * colour)
	{
		// WIP
		auto v_mag = state->m_state->vel.mag();

		auto phase = std::min(1.0, v_mag * 1e-5);

		auto red = static_cast<sf::Uint8>(m_cols[0].r + (m_cols[1].r - m_cols[0].r) * phase);
		auto green = static_cast<sf::Uint8>(m_cols[0].g + (m_cols[1].g - m_cols[0].g) * phase);
		auto blue = static_cast<sf::Uint8>(m_cols[0].b + (m_cols[1].b - m_cols[0].b) * phase);

		colour->colour = sf::Color{ red, green, blue };
	}
}