#include "BHTreeNode.h"
#include "Display.h"
#include "QuadManager.h"

#include <cassert>

namespace nbody
{
	QuadManager::QuadManager() : m_vtx_array(sf::Lines)
	{

	}

	QuadManager::~QuadManager()
	{

	}
	void QuadManager::update(BHTreeNode const* root, GridDrawMode mode)
	{
		m_vtx_array.clear();

		drawNode(root, mode);
	}

	void QuadManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(m_vtx_array);
	}

	void QuadManager::drawNode(BHTreeNode const* node, GridDrawMode mode)
	{
		assert(node);
		auto const& quad = node->getQuad();
		auto world_len = quad.getLength();
		auto world_pos = quad.getPos();
		auto screen_length = Display::worldToScreenLength(world_len);
		auto half_length = screen_length / 2;
		auto screen_x = Display::worldToScreenX(world_pos.x);
		auto screen_y = Display::worldToScreenY(world_pos.y);
		auto is_visible = (screen_length > 5)
			&& (screen_x + half_length > 0)
			&& (screen_x - half_length < Display::screen_size.x)
			&& (screen_y + half_length > 0)
			&& (screen_y - half_length < Display::screen_size.y);

		auto col{ (mode == GridDrawMode::COMPLETE) ? sf::Color::Green : sf::Color::Red };

		if (is_visible && (mode == GridDrawMode::COMPLETE || (mode == GridDrawMode::APPROX && !node->wasTooClose())))
		{
			// left edge
			m_vtx_array.append({{ screen_x - half_length, screen_y - half_length }, col});
			m_vtx_array.append({{ screen_x - half_length, screen_y + half_length }, col});
			// top edge
			m_vtx_array.append({{ screen_x - half_length, screen_y - half_length }, col});
			m_vtx_array.append({{ screen_x + half_length, screen_y - half_length }, col});
			// right edge
			m_vtx_array.append({{ screen_x + half_length, screen_y - half_length }, col});
			m_vtx_array.append({{ screen_x + half_length, screen_y + half_length }, col});
			// bottom edge
			m_vtx_array.append({{ screen_x + half_length, screen_y + half_length }, col});
			m_vtx_array.append({{ screen_x - half_length, screen_y + half_length }, col});
		}

		if (mode == GridDrawMode::APPROX && !node->wasTooClose())
			return;

		for (auto d : node->m_daughters)
		{
			if (d)
				drawNode(d, mode);
		}
	}
}
