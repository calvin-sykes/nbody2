#include "ColourerSolid.h"

namespace nbody
{
	ColourerSolid::ColourerSolid() : IColourer(1)
	{
	}

	ColourerSolid::~ColourerSolid()
	{
	}

	std::unique_ptr<IColourer> ColourerSolid::create()
	{
		return std::make_unique<ColourerSolid>();
	}

	void ColourerSolid::applyImpl(ParticleData const * state, ParticleColourState * colour)
	{
		colour->colour = m_cols[0];
	}
}