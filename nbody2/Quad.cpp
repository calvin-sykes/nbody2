#include "Quad.h"
#include "Constants.h"

namespace nbody
{
	StaticFont Quad::label_font("media/segoeui.ttf");
	
	Quad::Quad(const double x_midIn, const double y_midIn, const double lengthIn) : mid_point{ x_midIn, y_midIn }, length(lengthIn)
	{
		b_box.setOutlineColor(sf::Color::Green);
		b_box.setOutlineThickness(-1);
		b_box.setFillColor(sf::Color::Transparent);

		label.setFont(label_font.font);
		label.setCharacterSize(20);
		label.setFillColor(sf::Color::Green);
	}

	bool Quad::contains(Vector2d const & pt) const
	{
		auto half_length = length / 2;
		auto bounds_x = abs(pt.x - mid_point.x) < half_length;
		auto bounds_y = abs(pt.y - mid_point.y) < half_length;
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
		
		if(!contains(pt))
			return NONE;
		else
			return Daughter(((pt.x > mid_point.x) << 0) | ((pt.y > mid_point.y) << 1));
	}

	Quad Quad::makeDaughter(Daughter which) const
	{
		auto x_direction = (which >> 0) & 1 ? 1 : -1;
		auto y_direction = (which >> 1) & 1 ? 1 : -1;
		return Quad(mid_point.x + x_direction * length * 0.25, mid_point.y + y_direction * length * 0.25, length * 0.5);
	}

	void Quad::updateGfx(const size_t level)
	{
		screen_length = WORLD_TO_SCREEN_SIZE(length);
		auto half_length = screen_length / 2;
		// is quad visible?
		auto screen_x = WORLD_TO_SCREEN_X(mid_point.x);
		auto screen_y = WORLD_TO_SCREEN_Y(mid_point.y);
		is_visible = (screen_length > 5)
			&& (screen_x + half_length > 0)
			&& (screen_x - half_length < screen_size.x)
			&& (screen_y + half_length > 0)
			&& (screen_y - half_length < screen_size.y);

		if (is_visible)
		{
			b_box.setSize(sf::Vector2f(screen_length, screen_length));
			// place transformation origin in centre of box
			b_box.setOrigin(half_length, half_length);

			// translate box to correct screen position
			b_box.setPosition(screen_x, screen_y);

			if (screen_length > 20 && level != static_cast<size_t>(-1))
			{
				label.setString(std::to_string(level));
				label.setPosition(screen_x - half_length, screen_y - half_length);
				draw_label = true;
			}
			else
				draw_label = false;
		}
	}

	void Quad::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		if (is_visible)
		{
			target.draw(b_box);
			if (draw_label)
			{
				target.draw(label);
			}
		}
	}
}