#include "ColourerRealistic.h"
#include "Constants.h"
#include "specrend.h"

namespace nbody
{
	ColourerRealistic::ColourerRealistic() : IColourer(), m_temperatures(nullptr)
	{
	}

	ColourerRealistic::~ColourerRealistic()
	{
		delete[] m_temperatures;
		delete[] m_mapping;
	}

	std::unique_ptr<IColourer> ColourerRealistic::create()
	{
		return std::make_unique<ColourerRealistic>();
	}

	void ColourerRealistic::setup(size_t const offset, size_t const num_bodies, sf::Color const * cols, const ParticleData* state)
	{
		IColourer::setup(offset, num_bodies, cols);

		m_temperatures = new double[num_bodies];
		m_mapping = new size_t[num_bodies];

		// get temperatures from masses


		// calculate colour as fn of temperatures
		
		using namespace specrend;

		DecimalColour col;
		double x, y, z;
		auto cs = &EBUsystem;		
		for(auto i = 0; i < N_COLS; i++)
		{
			bbTemp = MIN_TEMP + TEMP_STEP * i;
			spectrum_to_xyz(bb_spectrum, &x, &y, &z);
			xyz_to_rgb(cs, x, y, z, &col.r, &col.g, &col.b);
			constrain_rgb(&col.r, &col.g, &col.b);
			norm_rgb(&col.r, &col.g, &col.b);

			m_colours[i] = { static_cast<sf::Uint8>(col.r * 255),
							 static_cast<sf::Uint8>(col.g * 255),
							 static_cast<sf::Uint8>(col.b * 255) };
		}
	}

	void ColourerRealistic::applyImpl(ParticleData const * state, ParticleColourState * colour)
	{
		auto temp = T_SUN * pow(state->m_aux_state->mass / Constants::SOLAR_MASS, 0.875);
		if (temp < MIN_TEMP)
			temp = MIN_TEMP;
		else if (temp > MAX_TEMP)
			temp = MAX_TEMP;

		auto col_idx = static_cast<size_t>((temp - MIN_TEMP) / TEMP_STEP);		
		
		colour->colour = m_colours[col_idx];
	}
}