#ifndef BODY2D_H
#define BODY2D_H

#include "BodyTrail.h"
#include "Vector.h"

#include <SFML/Graphics.hpp>

#include <memory>

namespace nbody
{
	class Integrator;

	class Body2d : public sf::Drawable
	{
	public:
		Body2d() = default;

		Body2d(Vector2d const& posIn, Vector2d const& velIn,  double const massIn, size_t const idIn);
		Body2d(Vector2d const& posIn, Vector2d const& velIn, double const massIn) : Body2d(posIn, velIn, massIn, id_counter++) {}

		~Body2d() = default;

		void update(double const dt);
		void addAccel(Vector2d const& other_pos, double const other_mass);
		void addAccel(Body2d const& other_body);
		void resetAccel();

		static Integrator * integrator_ptr;

		bool operator==(Body2d const& other) const;
		bool operator!=(Body2d const& other) const { return !operator==(other); };

		inline double getMass() const { return mass; }
		inline Vector2d getPos() const { return pos; }

		void updateGfx(bool const show_trails);
		void resetTrail();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	private:
		size_t id;
		static size_t id_counter;

		Vector2d pos;
		Vector2d vel;
		Vector2d acc;
		double mass;

		BodyTrail trail;
		sf::CircleShape gfx;
		bool is_visible;

		float static const MIN_SIZE;
		float static const MAX_SIZE;

	};
}

#endif // BODY2D_H

