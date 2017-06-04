#ifndef BODY_MANAGER_H
#define BODY_MANAGER_H

#include "Constants.h"

#include <SFML/Graphics.hpp>

#include <vector>

namespace nbody
{
	template<typename T, size_t N>
	class Vector;
	
	using Vector2d = Vector<double, 2>;

	struct ParticleState;
	struct ParticleAuxState;

	class BodyManager : public sf::Drawable
	{
	public:
		BodyManager();
		~BodyManager();

		void update(Vector2d const* state, ParticleAuxState const* aux_state, size_t const num_bodies);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		void drawBody(ParticleState const& p, size_t const idx);
		float radiusFromMass(double mass);

		float static constexpr MIN_SIZE = 1;
		float static constexpr MAX_SIZE = 5;
		float scl;

		size_t static constexpr N_VERTICES = 10;
		float static constexpr THETA = (2 * Constants::PI) / N_VERTICES;
		sf::Vector2f UNIT_CIRCLE[N_VERTICES];
		sf::Vector2f scratch[N_VERTICES];
		sf::VertexArray vtx_array;

		std::vector<float> radii;
		bool first_update;
	};
}

#endif // BODY_MANAGER_H

