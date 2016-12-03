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

		this->tree_ptr = nullptr;
		Body2d::integrator_ptr = this->sim->integrator_ptr;
	}

	void RunState::update(sf::Time const dt)
	{
		ImGui::SFML::Update(this->sim->window, dt);
		if (running || tree_old)
			this->sim->evolver_ptr->step(this->sim->bodies, this);

		ImGui::ShowMetricsWindow();
	}

	void RunState::draw(sf::Time const dt)
	{		
		if (view_centre)
		{
			Vector2f com_screen(Display::worldToScreenX(com.x), Display::worldToScreenY(com.y));
			Display::screen_offset += com_screen - 0.5f * Display::screen_size;
		}
		this->sim->window.setView(this->main_view);

		if (show_bodies)
		{
			for (auto& b : this->sim->bodies)
			{
				b.updateGfx(show_trails);
				this->sim->window.draw(b);
			}
		}

		if (show_grid && tree_ptr)
		{
			tree_ptr->updateGfx(show_grid_levels);
			this->sim->window.draw(*tree_ptr);
		}

		// finished with tree; it is regenerated from scratch next time
		// delete to prevent memory leak
		if (running || tree_old)
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
						running = !running;
					}
					else if (event.key.code == sf::Keyboard::C)
					{
						view_centre = !view_centre;
					}
					else if (event.key.code == sf::Keyboard::G)
					{
						show_grid = !show_grid;
					}
					else if (event.key.code == sf::Keyboard::L && show_grid)
					{
						show_grid_levels = !show_grid_levels;
					}
					else if (event.key.code == sf::Keyboard::B)
					{
						show_bodies = !show_bodies;
						for (auto& b : this->sim->bodies)
							b.resetTrail();
					}
					else if (event.key.code == sf::Keyboard::T && show_bodies)
					{
						show_trails = !show_trails;
						for (auto& b : this->sim->bodies)
							b.resetTrail();
					}
					else if (event.key.code == sf::Keyboard::R)
					{
						view_centre = false;
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
					if (view_dragging)
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
					if (!view_dragging)
					{
						view_centre = false;
						view_dragging = true;
						new_mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					// if dragging, stop
					if (view_dragging)
					{
						view_dragging = false;
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