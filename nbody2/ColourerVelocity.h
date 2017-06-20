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

		void setup(size_t const offset, size_t const num_bodies, sf::Color const* cols, ParticleState const* state) override;

		void applyImpl(ParticleData const* state, ParticleColourState * colour) override;

	private:
		double m_max_vel;
	};
}

#endif // COLOURER_VELOCITY_H