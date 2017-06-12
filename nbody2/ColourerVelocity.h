#ifndef COLOURER_VELOCITY_H
#define COLOURER_VELOCITY_H

#include "IColourer.h"

namespace nbody
{
	class ColourerVelocity : public IColourer
	{
	public:
		ColourerVelocity();
		virtual ~ColourerVelocity();

		static std::unique_ptr<IColourer> create();

		void applyImpl(ParticleData const* state, ParticleColourState * colour) override;
	};
}

#endif // COLOURER_VELOCITY_H