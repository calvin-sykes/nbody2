#include "ColourerSolid.h"

namespace nbody
{
	ColourerSolid::ColourerSolid(size_t offset, size_t num_bodies, sf::Color const* cols)
	: IColourer(offset, num_bodies, cols, 1)
	{
	}

	ColourerSolid::~ColourerSolid()
	{
	}

	std::unique_ptr<IColourer> ColourerSolid::create(size_t offset, size_t num_bodies, sf::Color const* cols)
	{
		return std::make_unique<ColourerSolid>(offset, num_bodies, cols);
	}

	void ColourerSolid::applyImpl(ParticleData const * state, ParticleColourState * colour)
	{
		colour->colour = m_cols[0];
	}
}