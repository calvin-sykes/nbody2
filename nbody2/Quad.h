#pragma once

#include "Vector.h"
#include "Error.h"

#include <SFML/Graphics.hpp>

namespace nbody
{
	typedef struct StaticFont
	{
		StaticFont(const char *fn)
		{
			if (!font.loadFromFile(fn))
				throw MAKE_ERROR("Font loading failed.");
		}

		operator sf::Font()
		{
			return font;
		}

		sf::Font font;
	} StaticFont;

	enum Daughter
	{
		// order matters!
		SW,
		SE,
		NW,
		NE,
		NONE = -1
	};
	constexpr int N_DAUGHTERS = 4;

	class Quad : public sf::Drawable
	{
		friend class BHTree;
	public:
		Quad() = default;
		Quad(const double x_midIn, const double y_midIn, const double lengthIn);
		~Quad() = default;

		double getLength() const { return length; }
		double getXmid() const { return mid_point.x; }
		double getYmid() const { return mid_point.y; }

		bool contains(Vector2d const& pt) const;
		Daughter whichDaughter(Vector2d const& pt) const;
		Quad makeDaughter(Daughter which) const;

		void updateGfx(const size_t level);
		void draw(sf::RenderTarget & target, sf::RenderStates states) const;

	private:
		double length;
		Vector2d mid_point;// x_mid, y_mid, length;

		sf::Text label;
		sf::RectangleShape b_box;
		float screen_length;
		bool is_visible, draw_label;

		static StaticFont label_font;
	};
}