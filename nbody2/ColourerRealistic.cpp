#include "ColourerRealistic.h"
#include "Constants.h"
#include "specrend.h"

#include <cassert>

namespace nbody
{
	ColourerRealistic::ColourerRealistic() : IColourer(), m_colours(nullptr)
	{
	}

	ColourerRealistic::~ColourerRealistic()
	{
		delete[] m_colours;
	}

	std::unique_ptr<IColourer> ColourerRealistic::create()
	{
		return std::make_unique<ColourerRealistic>();
	}

	void ColourerRealistic::setup(size_t const offset, size_t const num_bodies, sf::Color const * cols, const ParticleData* state)
	{
		IColourer::setup(offset, num_bodies, cols);

		m_colours = new sf::Color[num_bodies];
		sf::Color colours[N_COLS];

		using namespace specrend;

		DecimalColour col;
		double x, y, z;
		auto cs = &SMPTEsystem;
		for (auto i = 0; i < N_COLS; i++)
		{
			bbTemp = MIN_TEMP + TEMP_STEP * i;
			spectrum_to_xyz(bb_spectrum, &x, &y, &z);
			xyz_to_rgb(cs, x, y, z, &col.r, &col.g, &col.b);
			constrain_rgb(&col.r, &col.g, &col.b);
			norm_rgb(&col.r, &col.g, &col.b);

			colours[i] = { static_cast<sf::Uint8>(col.r * 255),
						   static_cast<sf::Uint8>(col.g * 255),
						   static_cast<sf::Uint8>(col.b * 255),
						   255 };
		}

		for (auto i = 0; i < num_bodies; i++)
		{
			auto temp = T_SUN * pow(state->m_aux_state[i].mass / Constants::SOLAR_MASS, 0.875);
			if (temp < MIN_TEMP)
				temp = MIN_TEMP;
			if (temp > MAX_TEMP)
				temp = MAX_TEMP;

			auto col_idx = static_cast<size_t>(floor((temp - MIN_TEMP) / TEMP_STEP));

			m_colours[i] = colours[col_idx];

			// special case for black hole
			if (state->m_aux_state[i].mass / Constants::SOLAR_MASS > 1e5)
				m_colours[i] = { 70, 70, 70 };
		}
	}

	void ColourerRealistic::applyImpl(ParticleData const * state, ParticleColourState * colour)
	{
		static size_t idx = 0;

		colour->colour = m_colours[idx];

		if (++idx == m_num_bodies)
			idx = 0;
	}
}
