#ifndef COLOURER_SOLID_H
#define COLOURER_SOLID_H

#include "IColourer.h"

namespace nbody
{
	class ColourerSolid : public IColourer
	{
	public:
		ColourerSolid();
		virtual ~ColourerSolid();

		static std::unique_ptr<IColourer> create();

		void applyImpl(ParticleData const* state, ParticleColourState * colour) override;
	};
}

#endif // COLOURER_SOLID_H