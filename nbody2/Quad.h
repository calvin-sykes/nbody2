#pragma once

#include "Vector.h"

namespace nbody
{

	enum Daughter
	{
		// order matters!
		SW,
		SE,
		NW,
		NE,
		NONE = -1
	};

	constexpr size_t NUM_DAUGHTERS = 4;

	class Quad
	{
	public:
		Quad() = default;
		Quad(Vector2d const& centre, double const length);
		~Quad() = default;

		double getLength() const { return m_length; }
		Vector2d getPos() const { return m_centre; }

		bool contains(Vector2d const& pt) const;
		Daughter whichDaughter(Vector2d const& pt) const;
		Quad createDaughter(Daughter which) const;

	private:
		double m_length;
		Vector2d m_centre;
	};
}