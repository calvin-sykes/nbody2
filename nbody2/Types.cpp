#include "Types.h"

#include <cassert>

namespace nbody
{
	ParticleData::ParticleData() :
		m_state(nullptr),
		m_aux_state(nullptr),
		m_deriv_state(nullptr)
	{
	}

	ParticleData::ParticleData(ParticleState * state, ParticleAuxState * aux_state, ParticleDerivState * deriv_state) :
		m_state(state),
		m_aux_state(aux_state),
		m_deriv_state(deriv_state)
	{
		assert(m_state);
		assert(m_aux_state);
	}

	ParticleData::ParticleData(ParticleState const* state, ParticleAuxState const* aux_state, ParticleDerivState const* deriv_state) :
		m_state(const_cast<ParticleState*>(state)),
		m_aux_state(const_cast<ParticleAuxState*>(aux_state)),
		m_deriv_state(const_cast<ParticleDerivState*>(deriv_state))
	{
		assert(m_state);
		assert(m_aux_state);
	}

	ParticleData::ParticleData(ParticleData const & src) :
		m_state(src.m_state),
		m_aux_state(src.m_aux_state),
		m_deriv_state(src.m_deriv_state)
	{
	}

	ParticleData & ParticleData::operator=(ParticleData const & src)
	{
		if (this != &src)
		{
			m_state = src.m_state;
			m_aux_state = src.m_aux_state;
			m_deriv_state = src.m_deriv_state;
		}

		return *this;
	}

	void ParticleData::reset()
	{
		m_state = nullptr;
		m_aux_state = nullptr;
		m_deriv_state = nullptr;
	}

	bool ParticleData::isNotNull() const
	{
		return (m_state && m_aux_state);
	}
}