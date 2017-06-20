#ifndef COLOURER_REALISTIC_H
#define COLOURER_REALISTIC_H

#include "IColourer.h"

namespace nbody
{
	struct DecimalColour
	{
		double r, g, b;
	};
	
	class ColourerRealistic : public IColourer
	{
	public:
		ColourerRealistic();
		virtual ~ColourerRealistic();

		static std::unique_ptr<IColourer> create();

		void setup(size_t const offset, size_t const num_bodies, sf::Color const* cols, const ParticleData* state) override;

		void applyImpl(ParticleData const* state, ParticleColourState * colour) override;

		double * m_temperatures;
		
		static constexpr size_t N_COLS = 200;
		static constexpr double MIN_TEMP = 1000;
		static constexpr double MAX_TEMP = 50000;
		static constexpr double TEMP_STEP = (MAX_TEMP - MIN_TEMP) / N_COLS;

		static constexpr double T_SUN = 5778;

		sf::Color m_colours[N_COLS];

		size_t * m_mapping;
	};
}

#endif // COLOURER_REALISTIC_H