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
		N_TYPES,
		INVALID = -1
	};

	struct ColourerProperties
	{
		ColourerProperties(ColourerType type, char const* name, char const* tooltip, char cols_used)
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

	size_t constexpr MAX_COLS_PER_COLOURER = 2;

	

	class IColourer
	{
	public:
		explicit IColourer(size_t offset, size_t num_bodies, sf::Color const* cols, size_t n_cols = MAX_COLS_PER_COLOURER);
		virtual ~IColourer();

		void apply(ParticleState const* state, ParticleAuxState const* aux_state, ParticleColourState * colours);
		virtual void applyImpl(ParticleData const* state, ParticleColourState * colour) = 0;

	protected:
		sf::Color m_cols[MAX_COLS_PER_COLOURER];

	private:
		size_t const m_offset, m_num_bodies;
		size_t m_n_cols;

	};
}

#endif // COLOURER_H