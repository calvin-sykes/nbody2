#include "BHTreeNode.h"
#include "Display.h"
#include "IState.h"
#include "ModelBarnesHut.h"
#include "RunState.h"
#include "Sim.h"
#include "Timings.h"

#include "imgui.h"
#include "imgui_sfml.h"

#include <SFML/Graphics.hpp>

namespace nbody
{	
	std::map<Timings, std::chrono::time_point<Clock>> timings;
	
	RunState::RunState(Sim * simIn) : m_highlighted(nullptr)
	{
		m_sim = simIn;
		auto pos = sf::Vector2f(m_sim->m_window.getSize());
		m_main_view.setSize(pos);
		m_gui_view.setSize(pos);
		m_main_view.setCenter(0.5f * pos);
		m_gui_view.setCenter(0.5f * pos);

		m_flags.tree_exists = m_sim->m_mod_ptr->hasTree();

		m_sim->m_mod_ptr->updateColours(m_sim->m_int_ptr->getState());
	}

	void RunState::update(sf::Time const dt)
	{
		if (m_flags.running)
		{
			m_sim->m_int_ptr->singleStep();
			m_sim->m_mod_ptr->updateColours(m_sim->m_int_ptr->getState());
		}

		timings[Timings::DRAW_BODIES_START] = Clock::now();
		if (m_flags.show_bodies)
		{
			m_body_mgr.update(
				m_sim->m_int_ptr->getState(),
				m_sim->m_mod_ptr->getAuxState(),
				m_sim->m_mod_ptr->getColourState(),
				m_sim->m_mod_ptr->getNumBodies());
		}
		timings[Timings::DRAW_BODIES_END] = Clock::now();

		timings[Timings::DRAW_GRID_START] = Clock::now();
		if (m_flags.tree_exists && m_flags.show_grid)
		{
			auto mouse_pos = sf::Mouse::getPosition(m_sim->m_window);
			auto mouse_world = Vector2d{ Display::screenToWorldX(static_cast<float>(mouse_pos.x)), Display::screenToWorldY(static_cast<float>(mouse_pos.y)) };
			m_highlighted = m_sim->m_mod_ptr->getTreeRoot()->getHovered(mouse_world);

			// if only drawing nodes used for force calculation
			// walk highlight pointer up tree until it points to a node not recursed into
			if (!m_flags.grid_mode_complete && m_highlighted)
			{
				while (!m_highlighted->getParent()->wasSubdivided())
					m_highlighted = m_highlighted->getParent();
			}

			auto mode = m_flags.grid_mode_complete ? GridDrawMode::COMPLETE : GridDrawMode::APPROX;
			m_quad_mgr.update(m_sim->m_mod_ptr->getTreeRoot(), mode, m_highlighted);
		}
		timings[Timings::DRAW_GRID_END] = Clock::now();

		timings[Timings::DRAW_TRAILS_START] = Clock::now();
		if (m_flags.show_trails)
		{
			m_trail_mgr.update(
				m_sim->m_int_ptr->getState(),
				m_sim->m_mod_ptr->getNumBodies());
		}
		timings[Timings::DRAW_TRAILS_END] = Clock::now();

		using namespace ImGui;

		SFML::Update(m_sim->m_window, dt);

		Begin("Diagnostics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

		if (CollapsingHeader("Statistics"))
		{
			if (m_flags.tree_exists)
			{
				auto mod_bh_tree = reinterpret_cast<ModelBarnesHut *>(m_sim->m_mod_ptr.get());
				auto stats = mod_bh_tree->getTreeRoot()->getStats();
				auto num_bodies = m_sim->m_mod_ptr->getNumBodies();
				Text("Force calculations for particle 0 = %zu", stats.m_num_calc);
				if (stats.m_num_calc)
					ImGui::Text("Speed-up vs. brute-force case = %f", static_cast<double>(num_bodies * (num_bodies - 1)) / (2 * stats.m_num_calc * num_bodies));
				Text("Total nodes in tree = %zu", stats.m_node_ct);
				Text("Level of deepest node = %zu", stats.m_max_level);
				Text("Particles in tree = %zu", stats.m_body_ct);
				Text("Renegade particles = %zu", num_bodies - stats.m_body_ct);
			}
		}

		if(CollapsingHeader("Timings"))
		{
			using namespace std::chrono;
			
			auto fps = 1000.f / dt.asMilliseconds();
			auto t_tree = Dble_ms{ timings[Timings::TREE_BUILD_END] - timings[Timings::TREE_BUILD_START] };
			auto t_eval = Dble_ms{ timings[Timings::FORCE_CALC_END] - timings[Timings::FORCE_CALC_START] };
			auto t_body = Dble_ms{ timings[Timings::DRAW_BODIES_END] - timings[Timings::DRAW_BODIES_START] };
			auto t_grid = Dble_ms{ timings[Timings::DRAW_GRID_END] - timings[Timings::DRAW_GRID_START] };
			auto t_trail = Dble_ms{ timings[Timings::DRAW_TRAILS_END] - timings[Timings::DRAW_TRAILS_START] };
			Text("FPS: %f", fps);
			Text("Tree construction: %fms", t_tree.count());
			Text("Force evaluation: %fms", t_eval.count());
			Text("Draw bodies: %fms", t_body.count());
			Text("Draw grid: %fms", t_grid.count());
			Text("Draw trails: %fms", t_trail.count());
		}

		if (CollapsingHeader("Highlighted tree node"))
		{
			if (m_highlighted)
			{
				Text("Bodies contained: %zu", m_highlighted->getNumBodies());
			}
			else
				Text("Mouse over a tree node to see statistics");
		}
		End();

		ShowTestWindow();
	}

	void RunState::draw(sf::Time const dt)
	{
		if (m_flags.view_centre)
		{
			auto com = m_sim->m_mod_ptr->getCentreMass();
			auto com_screen = Vector2f{ Display::worldToScreenX(com.x), Display::worldToScreenY(com.y) };
			Display::screen_offset += com_screen - 0.5f * Display::screen_size;
		}

		m_sim->m_window.setView(m_main_view);

		if (m_flags.show_bodies)
		{
			m_sim->m_window.draw(m_body_mgr);
		}

		if (m_flags.tree_exists && m_flags.show_grid)
		{
			m_sim->m_window.draw(m_quad_mgr);
		}

		if (m_flags.show_trails)
		{
			m_sim->m_window.draw(m_trail_mgr);
		}

		m_sim->m_window.setView(m_gui_view);

		ImGui::Render();
	}

	void RunState::handleInput()
	{
		sf::Event event;
		sf::Vector2i static prev_mouse_pos, new_mouse_pos;
		while (m_sim->m_window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (!ImGui::GetIO().WantCaptureMouse)
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					m_sim->m_window.close();
					break;
				}
				case sf::Event::Resized:
				{
					Display::screen_size = { static_cast<float>(event.size.width), static_cast<float>(event.size.height) };
					Display::aspect_ratio = static_cast<float>(event.size.width) / static_cast<float>(event.size.height);
					m_gui_view.setSize(Display::screen_size);
					m_gui_view.setCenter(Display::screen_size * 0.5f);
					m_main_view.setSize(Display::screen_size);
					m_main_view.setCenter(Display::screen_size * 0.5f);
					break;
				}
				case sf::Event::KeyPressed:
				{
					if (event.key.code == sf::Keyboard::Space)
					{
						m_flags.running = !m_flags.running;
					}
					else if (event.key.code == sf::Keyboard::C)
					{
						m_flags.view_centre = !m_flags.view_centre;
					}
					else if (event.key.code == sf::Keyboard::G)
					{
						m_flags.show_grid = !m_flags.show_grid;
					}
					else if (event.key.code == sf::Keyboard::M && m_flags.show_grid)
					{
						m_flags.grid_mode_complete = !m_flags.grid_mode_complete;
					}
					else if (event.key.code == sf::Keyboard::B)
					{
						m_flags.show_bodies = !m_flags.show_bodies;
					}
					else if (event.key.code == sf::Keyboard::T && m_flags.show_bodies)
					{
						m_flags.show_trails = !m_flags.show_trails;
						m_trail_mgr.reset();
					}
					else if (event.key.code == sf::Keyboard::R)
					{
						m_flags.view_centre = false;
						Display::screen_scale = 1;
						Display::screen_offset = { 0, 0 };
					}
					else if (event.key.code == sf::Keyboard::Escape)
					{
						m_sim->popState();
					}
					break;
				}
				case sf::Event::MouseWheelScrolled:
				{
					auto old_scale = Display::screen_scale;
					// calculate new display scale
					Display::screen_scale *= pow(2.f, -0.1f * event.mouseWheelScroll.delta);
					// shift offsets to keep view centred on same world position
					Display::screen_offset *= old_scale / Display::screen_scale;
					break;
				}
				case sf::Event::MouseMoved:
				{
					// if dragging, update pos
					if (m_flags.view_dragging)
					{
						prev_mouse_pos = new_mouse_pos;
						new_mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
						Display::screen_offset += Vector2f{ new_mouse_pos - prev_mouse_pos };
					}
					break;
				}
				case sf::Event::MouseButtonPressed:
				{
					// if not dragging, start
					if (!m_flags.view_dragging)
					{
						m_flags.view_centre = false;
						m_flags.view_dragging = true;
						new_mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					// if dragging, stop
					if (m_flags.view_dragging)
					{
						m_flags.view_dragging = false;
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}
