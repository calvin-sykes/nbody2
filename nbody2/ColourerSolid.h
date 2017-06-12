#ifndef COLOURER_SOLID_H
#define COLOURER_SOLID_H

#include "IColourer.h"

namespace nbody
{
	class ColourerSolid : public IColourer
	{
	public:
		ColourerSolid(size_t offset, size_t num_bodies, sf::Color const* cols);
		virtual ~ColourerSolid();

		static std::unique_ptr<IColourer> create(size_t offset, size_t num_bodies, sf::Color const* cols);

		void applyImpl(ParticleData const* state, ParticleColourState * colour) override;
	};
}

#endif // COLOURER_SOLID_H