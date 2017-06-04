#pragma once

#include "Vector.h"
#include "Error.h"

namespace nbody
{
	enum class Daughter
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
		Quad(const double x_midIn, const double y_midIn, const double lengthIn);
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