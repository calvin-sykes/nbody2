#ifndef QUAD_MANAGER_H
#define QUAD_MANAGER_H

#include <SFML/Graphics.hpp>

#include <vector>

namespace nbody
{
	enum class GridDrawMode
	{
		COMPLETE,
		APPROX
	};
	
	class BHTreeNode;
	class QuadManager : public sf::Drawable
	{
	public:
		QuadManager();
		~QuadManager();

		void update(BHTreeNode const* root, GridDrawMode mode);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		void drawNode(BHTreeNode const* node, GridDrawMode mode);

		sf::VertexArray m_vtx_array;
	};
}

#endif // QUAD_MANAGER_H
