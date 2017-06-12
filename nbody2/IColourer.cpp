#include "IColourer.h"

namespace nbody
{
	IColourer::IColourer(size_t offset, size_t num_bodies, sf::Color const* cols, size_t n_cols) :
		m_offset(offset), m_num_bodies(num_bodies), m_n_cols(n_cols)
	{
		std::copy(&cols[0], &cols[0] + MAX_COLS_PER_COLOURER, &m_cols[0]);
	}

	IColourer::~IColourer()
	{
	}

	void IColourer::apply(ParticleState const* state, ParticleAuxState const* aux_state, ParticleColourState * colours)
	{
		for(auto i = m_offset; i < m_offset + m_num_bodies; i++)
		{
			auto p = ParticleData{ const_cast<ParticleState*>(&state[i]), const_cast<ParticleAuxState*>(&aux_state[i]) };
			applyImpl(&p, colours + i);
		}
	}
}