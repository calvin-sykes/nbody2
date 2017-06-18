#include "BHTreeNode.h"
#include "Display.h"
#include "QuadManager.h"

#include <cassert>

namespace nbody
{
	const sf::Color QuadManager::s_highlight_colour = { 200, 200, 200, 100 };
	
	QuadManager::QuadManager()
		: m_vtx_array(sf::Lines),
		m_highlight_array(sf::Quads)
	{

	}

	QuadManager::~QuadManager()
	{

	}
	void QuadManager::update(BHTreeNode const* root, GridDrawMode mode, BHTreeNode const* highlighted)
	{
		m_vtx_array.clear();
		m_highlight_array.clear();

		drawNode(root, mode, highlighted);
	}

	void QuadManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(m_vtx_array);
		target.draw(m_highlight_array);
	}

	void QuadManager::drawNode(BHTreeNode const* node, GridDrawMode mode, BHTreeNode const* highlighted)
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

		// If complete grid is being drawn, then as long as the node is on screen it needs drawing
		// If force approx. grid is being drawn, don't draw nodes which were recursed into to calculate forces
		if (is_visible && (mode == GridDrawMode::COMPLETE || (mode == GridDrawMode::APPROX && !node->wasSubdivided())))
		{
			// left edge
			m_vtx_array.append({ { screen_x - half_length, screen_y - half_length }, col });
			m_vtx_array.append({ { screen_x - half_length, screen_y + half_length }, col });
			// top edge
			m_vtx_array.append({ { screen_x - half_length, screen_y - half_length }, col });
			m_vtx_array.append({ { screen_x + half_length, screen_y - half_length }, col });
			// right edge
			m_vtx_array.append({ { screen_x + half_length, screen_y - half_length }, col });
			m_vtx_array.append({ { screen_x + half_length, screen_y + half_length }, col });
			// bottom edge
			m_vtx_array.append({ { screen_x + half_length, screen_y + half_length }, col });
			m_vtx_array.append({ { screen_x - half_length, screen_y + half_length }, col });
			
			if (node == highlighted)
			{
				// top left
				m_highlight_array.append({ { screen_x - half_length, screen_y - half_length }, s_highlight_colour });
				// top right
				m_highlight_array.append({ { screen_x + half_length, screen_y - half_length }, s_highlight_colour });
				// bottom right
				m_highlight_array.append({ { screen_x + half_length, screen_y + half_length }, s_highlight_colour });
				// bottom left
				m_highlight_array.append({ { screen_x - half_length, screen_y + half_length }, s_highlight_colour });
			}
		}

		// If force approx. grid is being drawn, don't draw daughters if they weren't recursed into to calculate forces
		if (mode == GridDrawMode::APPROX && !node->wasSubdivided())
			return;

		for (auto d : node->m_daughters)
		{
			if (d)
				drawNode(d, mode, highlighted);
		}
	}
}
