#ifndef QUAD_MANAGER_H
#define QUAD_MANAGER_H

#include <SFML/Graphics.hpp>

#include <vector>

namespace nbody
{
	class BHTree;
	class QuadManager : public sf::Drawable
	{
	public:
		QuadManager();
		~QuadManager();

		void update(BHTree* ptr);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		sf::VertexArray vtx_array;
	};
}

#endif // QUAD_MANAGER_H
