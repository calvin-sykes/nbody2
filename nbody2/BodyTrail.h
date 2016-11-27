#ifndef BODYTRAIL_H
#define BODYTRAIL_H

#include "Vector.h"

#include <SFML/Graphics.hpp>

namespace nbody
{
	constexpr size_t NPOINTS = 10;

	class BodyTrail : public sf::Drawable
	{
	public:
		BodyTrail() = default;

		BodyTrail(const Vector2d& posIn);

		void update(const Vector2d& pos);
		void reset();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	private:
		Vector2d world_coords[NPOINTS];
		sf::VertexArray screen_coords;
		size_t last;
	};
}
#endif // !BODYTRAIL_H
