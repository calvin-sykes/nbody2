#ifndef TYPES_H
#define TYPES_H

#include "Vector.h"
#include <SFML/Graphics/Color.hpp>

#pragma pack(push, 1)

namespace nbody
{
	struct ParticleState
	{
		Vector2d pos;
		Vector2d vel;
	};

	struct ParticleAuxState
	{
		double mass;
	};

	struct ParticleColourState
	{
		sf::Color colour;
	};
	
	struct ParticleDerivState
	{
		Vector2d vel;
		Vector2d acc;
	};

#pragma pack(pop)

	struct ParticleData
	{
		ParticleData();
		ParticleData(ParticleState * state, ParticleAuxState * aux_state);
		ParticleData(ParticleData const& src);
		ParticleData & operator=(ParticleData const& src);

		bool operator==(ParticleData const& other) const;

		void reset();
		bool isNotNull() const;

		ParticleState * m_state;
		ParticleAuxState * m_aux_state;
	};
}

#endif // !TYPES_H