#ifndef COLOURER_H
#define COLOURER_H

#include "Types.h"

#include <memory>

namespace nbody
{
	enum class ColourerType
	{
		SOLID,
		VELOCITY,
		REALISTIC,
		N_TYPES,
		INVALID = -1
	};

	struct ColourerProperties
	{
		constexpr ColourerProperties(ColourerType const type, char const* name, char const* tooltip, char const cols_used)
			: type(type),
			  name(name),
			  tooltip(tooltip),
			  cols_used(cols_used)
		{
		}

		ColourerType const type;
		char const* name;
		char const* tooltip;
		char cols_used;
	};

	using CPArray = std::array<ColourerProperties, static_cast<size_t>(ColourerType::N_TYPES)>;

	constexpr CPArray m_colour_infos = { {
		{
			ColourerType::SOLID,
			"Single",
			"All bodies in this group are coloured the same",
			1
		},
		{
			ColourerType::VELOCITY,
			"Velocity",
			"Bodies in this group are coloured according to their velocity",
			2
		},
		{
			ColourerType::REALISTIC,
			"Realistic",
			"Bodies are coloured by the peak wavelength of the light they would emit, based on their mass",
			0
		}
		} };

	size_t constexpr MAX_COLS_PER_COLOURER = 2;

	class IColourer
	{
	public:
		explicit IColourer(size_t n_cols = MAX_COLS_PER_COLOURER);
		virtual ~IColourer();

		virtual void setup(size_t const offset, size_t const num_bodies, sf::Color const* cols, const ParticleData* = nullptr);
		
		void apply(ParticleState const* state, ParticleAuxState const* aux_state, ParticleColourState * colours);
		virtual void applyImpl(ParticleData const* state, ParticleColourState * colour) = 0;

	protected:
		sf::Color m_cols[MAX_COLS_PER_COLOURER];
		size_t m_num_bodies;

	private:
		size_t m_offset;
		size_t m_n_cols;

	};
}

#endif // COLOURER_H