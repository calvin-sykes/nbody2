#ifndef TYPES_H
#define TYPES_H

#include "Vector.h"
#include <SFML/Graphics/Color.hpp>

#pragma pack(push, 1)

namespace nbody
{
	struct ParticleState
	{
		ParticleState()
		{}

		ParticleState(Vector2d const& p, Vector2d const& v) :
			pos(p),
			vel(v)
		{}

		Vector2d pos;
		Vector2d vel;
	};

	struct ParticleAuxState
	{
		ParticleAuxState() :
			mass(0.0)
		{}

		explicit ParticleAuxState(double const m) :
			mass(m)
		{}

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
		ParticleData(ParticleState * state, ParticleAuxState * aux_state, ParticleDerivState * deriv_state = nullptr);
		ParticleData(ParticleState const* state, ParticleAuxState const* aux_state, ParticleDerivState const* deriv_state = nullptr);
		ParticleData(ParticleData const& src);
		ParticleData & operator=(ParticleData const& src);

		void reset();
		bool isNotNull() const;

		ParticleState * m_state;
		ParticleAuxState * m_aux_state;
		ParticleDerivState * m_deriv_state;
	};
}

#endif // !TYPES_H