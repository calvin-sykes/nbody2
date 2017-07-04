#include "Quad.h"

namespace nbody
{
	using namespace priv;
	
	Quad::Quad(Vector2d const& centre, double const length) :
		m_length(length),
		m_centre(centre)
	{
	}

	bool Quad::contains(Vector2d const & pt) const
	{
		auto half_length = m_length / 2;
		auto bounds_x = abs(pt.x - m_centre.x) < half_length;
		auto bounds_y = abs(pt.y - m_centre.y) < half_length;
		return bounds_x && bounds_y;
	}

	Daughter Quad::whichDaughter(Vector2d const & pt) const
	{
		//   Q	| y > mp.y	| x > mp.x
		// -----+-----------+---------
		//  SW	|	  0		|	 0
		//  SE	|	  0		|	 1
		//  NW	|	  1		|	 0
		//  NE	|	  1		|	 1

		if (!contains(pt))
			return NONE;
		else
			return Daughter((pt.x > m_centre.x) << 0 | (pt.y > m_centre.y) << 1);
	}

	Quad Quad::createDaughter(Daughter which) const
	{
		auto x_direction = (static_cast<size_t>(which) >> 0) & 1 ? 1 : -1;
		auto y_direction = (static_cast<size_t>(which) >> 1) & 1 ? 1 : -1;
		auto offset = Vector2d{ x_direction * m_length * 0.25, y_direction * m_length * 0.25 };
		return { m_centre + offset, m_length * 0.5 };
	}
}