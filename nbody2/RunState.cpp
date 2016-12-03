#include "BHTree.h"
#include "Constants.h"
#include "Display.h"
#include "Evolver.h"
#include "Quad.h"
#include "RunState.h"
#include "Sim.h"
#include "SimState.h"

#include "imgui.h"
#include "imgui_sfml.h"

#include <SFML/Graphics.hpp>

#include <Windows.h>

namespace nbody
{
	RunState::RunState(Sim * simIn)
	{
		this->sim = simIn;
		sf::Vector2f pos = sf::Vector2f(this->sim->window.getSize());
		this->main_view.setSize(pos);
		this->gui_view.setSize(pos);
		this->main_view.setCenter(0.5f * pos);
		this->gui_view.setCenter(0.5f * pos);

		flags.tree_exists = this->sim->evolver_ptr->has_tree;
		Body2d::integrator_ptr = this->sim->integrator_ptr;
	}

	void RunState::update(sf::Time const dt)
	{
		ImGui::SFML::Update(this->sim->window, dt);

		if (flags.running || flags.tree_exists && (flags.tree_old || flags.current_show_grid != flags.show_grid))
			this->sim->evolver_ptr->calcStep(this->sim->bodies, this->com, this->tree_ptr, this->flags);

		if (flags.running)
			this->sim->evolver_ptr->advanceStep(this->sim->bodies, this->com, this->tree_ptr, this->flags);

		ImGui::ShowMetricsWindow();
	}

	void RunState::draw(sf::Time const dt)
	{		
		if (flags.view_centre)
		{
			Vector2f com_screen(Display::worldToScreenX(com.x), Display::worldToScreenY(com.y));
			Display::screen_offset += com_screen - 0.5f * Display::screen_size;
		}

		this->sim->window.setView(this->main_view);

		if (flags.show_bodies)
		{
			for (auto& b : this->sim->bodies)
			{
				b.updateGfx(flags.show_trails);
				this->sim->window.draw(b);
			}
		}

		if (flags.tree_exists && flags.show_grid)
		{
			tree_ptr->updateGfx(flags.show_grid_levels);
			this->sim->window.draw(*tree_ptr);
		}

		// finished with tree; it is regenerated from scratch next time
		// delete to prevent memory leak
		if (flags.tree_exists && (flags.running || flags.tree_old))
		{
			delete tree_ptr;
		}

		this->sim->window.setView(this->gui_view);

		ImGui::Render();
	}

	void RunState::handleInput()
	{
		sf::Event event;
		sf::Vector2i static prev_mouse_pos, new_mouse_pos;
		while (this->sim->window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			
			if (!ImGui::GetIO().WantCaptureMouse)
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					this->sim->window.close();
					break;
				}
				case sf::Event::Resized:
				{
					Display::screen_size = Vector2f(event.size.width, event.size.height);
					Display::aspect_ratio = event.size.width / event.size.height;
					gui_view.setSize(Display::screen_size);
					gui_view.setCenter(Display::screen_size * 0.5f);
					main_view.setSize(Display::screen_size);
					main_view.setCenter(Display::screen_size * 0.5f);
					break;
				}
				case sf::Event::KeyPressed:
				{
					if (event.key.code == sf::Keyboard::Space)
					{
						flags.running = !flags.running;
					}
					else if (event.key.code == sf::Keyboard::C)
					{
						flags.view_centre = !flags.view_centre;
					}
					else if (event.key.code == sf::Keyboard::G)
					{
						flags.show_grid = !flags.show_grid;
					}
					else if (event.key.code == sf::Keyboard::L && flags.show_grid)
					{
						flags.show_grid_levels = !flags.show_grid_levels;
					}
					else if (event.key.code == sf::Keyboard::B)
					{
						flags.show_bodies = !flags.show_bodies;
						for (auto& b : this->sim->bodies)
							b.resetTrail();
					}
					else if (event.key.code == sf::Keyboard::T && flags.show_bodies)
					{
						flags.show_trails = !flags.show_trails;
						for (auto& b : this->sim->bodies)
							b.resetTrail();
					}
					else if (event.key.code == sf::Keyboard::R)
					{
						flags.view_centre = false;
						Display::screen_scale = 1;
						Display::screen_offset = { 0, 0 };
					}
					else if (event.key.code == sf::Keyboard::Escape)
					{
						this->sim->window.close();
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
					if (flags.view_dragging)
					{
						prev_mouse_pos = new_mouse_pos;
						new_mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
						Display::screen_offset += new_mouse_pos - prev_mouse_pos;
					}
					break;
				}
				case sf::Event::MouseButtonPressed:
				{
					// if not dragging, start
					if (!flags.view_dragging)
					{
						flags.view_centre = false;
						flags.view_dragging = true;
						new_mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					// if dragging, stop
					if (flags.view_dragging)
					{
						flags.view_dragging = false;
						//#ifdef NBOS_WINDOWS
						//					// reset mouse to screen centre
						//					auto hwnd = this->sim->window.getSystemHandle();
						//					RECT wnd_area;
						//					GetClientRect(hwnd, &wnd_area);
						//					ClientToScreen(hwnd, (LPPOINT)&wnd_area + 1);
						//					POINT sz = *(LPPOINT(&wnd_area) + 1);
						//					SetCursorPos(sz.x / 2, sz.y / 2);
						//#endif
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