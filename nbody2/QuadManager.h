#ifndef QUAD_MANAGER_H
#define QUAD_MANAGER_H

#include <SFML/Graphics.hpp>

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

		void update(BHTreeNode const* root, GridDrawMode mode, BHTreeNode const* highlighted = nullptr);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		void drawNode(BHTreeNode const* node, GridDrawMode mode, BHTreeNode const* highlighted = nullptr);

		sf::VertexArray m_vtx_array;
		sf::VertexArray m_highlight_array;

		static const sf::Color s_highlight_colour;
	};
}

#endif // QUAD_MANAGER_H
