#include "Body2d.h"
#include "Constants.h"

namespace nbody
{
	std::unique_ptr<Integrator> Body2d::p_integrator = nullptr;
	size_t Body2d::id_counter = 0;
	float const Body2d::MIN_SIZE = 2;
	float const Body2d::MAX_SIZE = 6;


	Body2d::Body2d(Vector2d const& posIn, Vector2d const& velIn, double const massIn, size_t const idIn)
		: pos(posIn), vel(velIn), acc(), mass(massIn), id(idIn), trail(posIn)
	{
		auto rad = min(static_cast<float>(MIN_SIZE * log10(mass / BODY_MASS)), MAX_SIZE);
		gfx = sf::CircleShape(static_cast<float>(rad), 20);
		gfx.setOrigin(static_cast<float>(rad), static_cast<float>(rad));
		gfx.setOutlineColor(sf::Color::White);
		gfx.setOutlineThickness(0.1f);
	};

	void Body2d::addAccel(Vector2d const& other_pos, double const other_mass)
	{
		auto rel_pos = other_pos - pos; // relative position vector r
		auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
		auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // r/|r|
		// F = (G m1 m2 / (|r|**2 + eps**2) * r_hat
		// a = F / m1
		acc += (G * other_mass / (rel_pos_mag_sq + EPS * EPS)) * unit_vec;
	}

	void Body2d::addAccel(Body2d const& other_body)
	{
		auto rel_pos = other_body.pos - pos; // relative position vector r
		auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
		auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // r/|r|
		// F = (G m1 m2 / (|r|**2 + eps**2) * r_hat
		// a = F / m1
		acc += (G * other_body.mass / (rel_pos_mag_sq + EPS * EPS)) * unit_vec;
	}

	void Body2d::resetAccel()
	{
		acc = {};
	}

	bool Body2d::operator==(Body2d const& other) const
	{
		return id == other.id;
	}

	void Body2d::update(double const dt)
	{
		p_integrator->step(id, pos, vel, acc, dt);
	}

	void Body2d::updateGfx(bool const show_trails)
	{
		auto screen_x = WORLD_TO_SCREEN_X(pos.x);
		auto screen_y = WORLD_TO_SCREEN_Y(pos.y);
		is_visible = screen_x < screen_size.x && screen_x > 0 && screen_y < screen_size.y && screen_y > 0;
		if (is_visible)
		{
			gfx.setPosition(screen_x, screen_y);
			if (show_trails)
				trail.update(pos);
			auto scale = max(static_cast<float>(-std::log2(screen_scale)), 1.f);
			gfx.setScale(scale, scale);
			auto v_mag = vel.mag();
			auto phase = min(PI / 2., (v_mag / 100000.) * (PI / 2.));
			auto red = (int)(254 * sin(phase));
			auto blue = (int)(254 * cos(phase));
			auto green = 0;
			gfx.setFillColor(sf::Color(red, green, blue));
		}
		else
			trail.reset();
	}

	void Body2d::resetTrail()
	{
		trail.reset();
	}

	void Body2d::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		if (is_visible)
		{
			target.draw(gfx);
			target.draw(trail);
		}
	}
}