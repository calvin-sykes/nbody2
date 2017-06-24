#include "Types.h"

#include <cassert>

namespace nbody
{
	ParticleData::ParticleData()
		: m_state(nullptr), m_aux_state(nullptr)
	{
	}

	ParticleData::ParticleData(ParticleState * state, ParticleAuxState * aux_state)
		: m_state(state), m_aux_state(aux_state)
	{
		assert(m_state);
		assert(m_aux_state);
	}

	ParticleData::ParticleData(ParticleData const & src)
		: m_state(src.m_state), m_aux_state(src.m_aux_state)
	{
	}

	ParticleData & ParticleData::operator=(ParticleData const & src)
	{
		if (this != &src)
		{
			m_state = src.m_state;
			m_aux_state = src.m_aux_state;
		}

		return *this;
	}

	bool ParticleData::operator==(ParticleData const& other) const
	{
		return m_state == other.m_state && m_aux_state == other.m_aux_state;
	}

	void ParticleData::reset()
	{
		m_state = nullptr;
		m_aux_state = nullptr;
	}

	bool ParticleData::isNotNull() const
	{
		return (m_state && m_aux_state);
	}
}