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
	struct ParticleColourState;

	class BodyManager : public sf::Drawable
	{
	public:
		BodyManager();
		~BodyManager();

		void update(Vector2d const* state, ParticleAuxState const* aux_state, ParticleColourState const* colour_state, size_t const num_bodies);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		void setDirty();

	private:
		void drawBody(ParticleState const& p, ParticleColourState const& c, size_t const idx);
		float radiusFromMass(double mass) const;

		float static constexpr s_MIN_SIZE = 1;
		float static constexpr s_MAX_SIZE = 5;
		float static constexpr s_BH_SIZE = 8;
		size_t static constexpr s_VERTICES = 20;
		size_t static constexpr s_VERTICES_SMALL = 10;
		float static constexpr s_THETA = static_cast<float>(2 * Constants::PI) / s_VERTICES;

		sf::Vector2f m_unit_circle[s_VERTICES];
		sf::Vector2f m_scratch[s_VERTICES];
		sf::VertexArray m_vtx_array;

		float m_scl;

		std::vector<float> m_radii;
		bool m_first_update;
	};
}

#endif // BODY_MANAGER_H

