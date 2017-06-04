#ifndef TYPES_H
#define TYPES_H

#include "Vector.h"

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
		bool isNull() const;

		ParticleState * m_state;
		ParticleAuxState * m_aux_state;
	};
}

#endif // !TYPES_H