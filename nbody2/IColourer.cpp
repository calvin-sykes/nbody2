#include "IColourer.h"

namespace nbody
{
	IColourer::IColourer(size_t n_cols) :
		m_offset(0), m_num_bodies(0), m_n_cols(n_cols)
	{
	}

	IColourer::~IColourer()
	{
	}

	void IColourer::setup(size_t const offset, size_t const num_bodies, sf::Color const * cols, const ParticleData* state)
	{
		m_offset = offset;
		m_num_bodies = num_bodies;
		std::copy(&cols[0], &cols[0] + MAX_COLS_PER_COLOURER, &m_cols[0]);
	}

	void IColourer::apply(ParticleState const* state, ParticleAuxState const* aux_state, ParticleColourState * colours)
	{
		for(auto i = m_offset; i < m_offset + m_num_bodies; i++)
		{
			auto p = ParticleData{ &state[i], &aux_state[i] };
			applyImpl(&p, colours + i);
		}
	}
}