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
	void QuadManager::update(BHTreeNode const* root, GridDrawMode mode)
	{
		m_vtx_array.clear();
		m_highlight_array.clear();

		drawNode(root, mode);
	}

	void QuadManager::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(m_vtx_array);
		target.draw(m_highlight_array);
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

		auto mouse_pos = sf::Mouse::getPosition();
		auto mouse_world = Vector2d{ Display::screenToWorldX(static_cast<float>(mouse_pos.x)), Display::screenToWorldY(static_cast<float>(mouse_pos.y)) };

		auto col{ (mode == GridDrawMode::COMPLETE) ? sf::Color::Green : sf::Color::Red };

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

			// first pass: is mouse in this quad?
			auto do_highlight = quad.contains(mouse_world);
			// second pass
			if (mode == GridDrawMode::COMPLETE)
				// highlight only if there is not a daughter node which also contains mouse
				do_highlight &= !node->m_daughters[static_cast<size_t>(quad.whichDaughter(mouse_world))];
			else
				// highlight only if node is being rendered
				do_highlight &= !node->wasSubdivided();
			
			if (do_highlight)
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

		if (mode == GridDrawMode::APPROX && !node->wasSubdivided())
			return;

		for (auto d : node->m_daughters)
		{
			if (d)
				drawNode(d, mode);
		}
	}
}
