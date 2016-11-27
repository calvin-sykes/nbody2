#include "Constants.h"
#include "Vector.h"
#include "Body2d.h"
#include "BHTree.h"
#include "Quad.h"
#include "BodyDistributor.h"

#include "Sim.h"
#include "StartState.h"
#include "Error.h"

#include <sstream>
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>

#define ICON_FNAME "media/sfml.png"
#define FONT_FNAME "media/segoeui.ttf"

namespace nbody
{
	float extern screen_scale = 1;
	float extern aspect_ratio = 0;
	Vector2f extern screen_size = {};
	Vector2f extern screen_offset = {};

	std::unique_ptr<std::array<Body2d, N_MAX>> generateBodies();
	BHTree* buildTree(size_t const n, std::array<Body2d, N_MAX> const* bodies, Quad const& root);
	BHTree* buildTreeThreaded(size_t const n, std::array<Body2d, N_MAX> const* bodies, Quad const& root);
	void stepBH(size_t const n, std::array<Body2d, N_MAX>* bodies, BHTree const* p_tree, Quad const& root);

	int mainBH()
	{
		auto running = false;
		auto show_bodies = true;
		auto show_grid = false;
		auto show_grid_levels = false;
		auto show_trails = false;
		auto current_show_grid = false;
		auto view_centre = true;
		auto view_dragging = false;
		auto tree_old = true;

		// create window //
#ifndef NDEBUG
		sf::RenderWindow window(sf::VideoMode(900, 900), "nbody2");
#else
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		auto mode = sf::VideoMode().getFullscreenModes()[0];
		sf::RenderWindow window(mode, "nbody2", sf::Style::Fullscreen, settings);
		//sf::RenderWindow window(sf::VideoMode(1600, 900), "nbody2");
#endif
		sf::Image icon;
		if (!icon.loadFromFile(ICON_FNAME))
			throw MAKE_ERROR("Icon " ICON_FNAME " not found.");
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		window.setVerticalSyncEnabled(true);
		window.setMouseCursorVisible(false);
		// set up views //
		screen_size = window.getSize();
		aspect_ratio = static_cast<float>(screen_size.x) / static_cast<float>(screen_size.y);
		sf::View gui_view(sf::FloatRect(sf::Vector2f(), screen_size));
		sf::View bodies_view(sf::FloatRect(sf::Vector2f(), screen_size));
		window.setView(bodies_view);
		auto bodies = generateBodies();
		Vector2d com;
		BHTree* p_tree;

		// choose integration method //
		Body2d::p_integrator = std::make_unique<EulerIntegrator>();

		// load font //
		sf::Font font;
		if (!font.loadFromFile(FONT_FNAME))
			throw MAKE_ERROR("Font " FONT_FNAME " not found.");
		// fps counter //
		sf::Text fps_ctr;
		fps_ctr.setFont(font);
		fps_ctr.setCharacterSize(20);
		fps_ctr.setFillColor(sf::Color::White);
		// debug text //
		sf::Text text;
		text.setFont(font);
		text.setCharacterSize(20);
		text.setFillColor(sf::Color::White);

		sf::Clock clock;

		while (window.isOpen())
		{
			auto initial = clock.getElapsedTime();

			/////////////////
			// POLL EVENTS //
			/////////////////
			sf::Event e;
			sf::Vector2i static prev_mouse_pos, new_mouse_pos;
			while (window.pollEvent(e))
			{
				switch (e.type)
				{
				case sf::Event::Closed:
				{
					window.close();
					break;
				}
				case sf::Event::Resized:
				{
					screen_size = Vector2f(e.size.width, e.size.height);
					aspect_ratio = screen_size.x / screen_size.y;
					gui_view.setSize(screen_size);
					gui_view.setCenter(screen_size * 0.5f);
					bodies_view.setSize(screen_size);
					bodies_view.setCenter(screen_size * 0.5f);
					break;
				}
				case sf::Event::KeyPressed:
				{
					if (e.key.code == sf::Keyboard::Space)
					{
						running = !running;
					}
					else if (e.key.code == sf::Keyboard::C)
					{
						view_centre = !view_centre;
					}
					else if (e.key.code == sf::Keyboard::G)
					{
						show_grid = !show_grid;
					}
					else if (e.key.code == sf::Keyboard::L && show_grid)
					{
						show_grid_levels = !show_grid_levels;
					}
					else if (e.key.code == sf::Keyboard::B)
					{
						show_bodies = !show_bodies;
						for (size_t i = 0; i < N; i++)
							(*bodies)[i].resetTrail();
					}
					else if (e.key.code == sf::Keyboard::T && show_bodies)
					{
						show_trails = !show_trails;
						for (size_t i = 0; i < N; i++)
							(*bodies)[i].resetTrail();
					}
					else if (e.key.code == sf::Keyboard::R)
					{
						view_centre = false;
						screen_scale = 1;
						screen_offset = { 0, 0 };
					}
					else if (e.key.code == sf::Keyboard::Escape)
					{
						window.close();
					}
					break;
				}
				case sf::Event::MouseWheelScrolled:
				{
					auto old_scale = screen_scale;
					// calculate new display scale
					screen_scale *= pow(2.f, -0.1f * e.mouseWheelScroll.delta);
					// shift offsets to keep view centred on same world position
					screen_offset *= old_scale / screen_scale;
					break;
				}
				case sf::Event::MouseMoved:
				{
					// if dragging, update pos
					if (view_dragging)
					{
						prev_mouse_pos = new_mouse_pos;
						new_mouse_pos = sf::Vector2i(e.mouseMove.x, e.mouseMove.y);
						screen_offset += new_mouse_pos - prev_mouse_pos;
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
						new_mouse_pos = sf::Vector2i(e.mouseButton.x, e.mouseButton.y);
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					// if dragging, stop
					if (view_dragging)
					{
						view_dragging = false;
						// reset mouse to screen centre
						auto hwnd = window.getSystemHandle();
						RECT wnd_area;
						GetClientRect(hwnd, &wnd_area);
						ClientToScreen(hwnd, (LPPOINT)&wnd_area + 1);
						POINT sz = *(LPPOINT(&wnd_area) + 1);
						SetCursorPos(sz.x / 2, sz.y / 2);
					}
					break;
				}
				default:
					break;
				}
			}

			//////////////
			// SIMULATE //
			//////////////

			// create root quadrant of tree
			Quad root(com.x, com.y, 10 * RAD);

			// reconstruct BH tree if needed
			if (tree_old || current_show_grid != show_grid)
			{
				p_tree = buildTreeThreaded(N, bodies.get(), root);
				com = p_tree->getPCoM();
				tree_old = false;
				current_show_grid = show_grid;
			}

			if (running)
			{
				// advance bodies
				stepBH(N, bodies.get(), p_tree, root);
				tree_old = true;
			}

			//////////
			// DRAW //
			//////////
			window.clear(sf::Color::Black);

			if (view_centre)
			{
				Vector2f com_screen(WORLD_TO_SCREEN_X(com.x), WORLD_TO_SCREEN_Y(com.y));
				screen_offset += com_screen - 0.5f * screen_size;
			}
			window.setView(bodies_view);

			if (show_bodies)
			{
				for (size_t i = 0; i < N; i++)
				{
					(*bodies)[i].updateGfx(show_trails);
					window.draw((*bodies)[i]);
				}
			}

			if (show_grid)
			{
				p_tree->updateGfx(show_grid_levels);
				window.draw(*p_tree);
			}

			auto final = clock.getElapsedTime();
			auto deltaT = final - initial;
			auto fps = static_cast<size_t>(1.f / deltaT.asSeconds());
			fps_ctr.setString(std::to_string(fps));
			std::stringstream builder;
			//builder << " X offset: " << static_cast<int>(WIN_OFFSET_X) << " Y offset: " << static_cast<int>(WIN_OFFSET_Y) << " Scale: " << screen_scale;
			text.setString(builder.str());
			text.setPosition(screen_size.x - text.getLocalBounds().width, 0);
			window.setView(gui_view);
			window.draw(fps_ctr);
			window.draw(text);

			window.display();

			// finished with tree; it is regenerated from scratch next time
			// delete to prevent memory leak
			if (running || tree_old)
			{
				delete p_tree;
			}
		}
		return 0;
	}

	std::unique_ptr<std::array<Body2d, N_MAX>> generateBodies()
	{
		size_t n1 = N; //(3 * N) / 4;
		size_t n2 = N - n1;
		auto r1 = 0.5 * RAD;
		auto r2 = 0.375 * RAD;
		auto cm1 = CENTRAL_MASS;
		auto cm2 = 0.75 * CENTRAL_MASS;
		auto pos1 = Vector2d(RAD / 2, 0.1 * RAD);
		auto pos2 = -2 * pos1;
		auto vel1 = Vector2d(-5000, -2000);
		auto vel2 = -1 * vel1;
		// populate array of bodies
		auto bodies = std::make_unique<std::array<Body2d, N_MAX>>();
		auto dist1 = ExponentialDistributor(r1, cm1, -1.8, pos1, vel1);
		auto dist2 = ExponentialDistributor(r2, cm2, -1.8, pos2, vel2);
		//auto dist1 = PlummerDistributor(r1, cm1, n1, pos1, vel1);
		//auto dist2 = PlummerDistributor(r2, cm2, n2, pos2, vel2);
		//auto dist1 = UniformDistributor(r1, cm1, pos1, vel1);
		//auto dist2 = UniformDistributor(r2, cm2, pos2, vel2);
		for (size_t i = 0; i < n1; i++)
			(*bodies)[i] = dist1.sample();
		// central mass
		(*bodies)[0] = Body2d(pos1, vel1, cm1, 0);
		//for (size_t i = n1; i < N; i++)
		//	(*bodies)[i] = dist2.sample();
		// central mass
		//(*bodies)[n1] = Body2d(pos2, vel2, cm2, n1);
		return bodies;
	}

	BHTree* buildTree(const size_t n, const std::array<Body2d, N_MAX>* bodies, const Quad& root)
	{
		auto p_tree = new BHTree(root, 0);
		for (size_t i = 0; i < n; i++)
			p_tree->insert(static_cast<pcBody>(&(*bodies)[i]));
		return p_tree;
	}

	BHTree* buildTreeThreaded(const size_t n, const std::array<Body2d, N_MAX>* bodies, const Quad& root)
	{
		// construct four top-level subtrees in parallel 
		BHTree* nwt, *net, *swt, *set;
#pragma omp parallel sections
		{
#pragma omp section
			{
				nwt = buildTree(n, bodies, root.makeDaughter(NW));
			}
#pragma omp section
			{
				net = buildTree(n, bodies, root.makeDaughter(NE));
			}
#pragma omp section
			{
				swt = buildTree(n, bodies, root.makeDaughter(SW));
			}
#pragma omp section
			{
				set = buildTree(n, bodies, root.makeDaughter(SE));
			}
		}
		return new BHTree(nwt, net, swt, set);
	}

	void stepBH(const size_t n, std::array<Body2d, N_MAX>* bodies, const BHTree* p_tree, const Quad& root)
	{
#pragma omp parallel for schedule(static)
		for (int i = 0; i < n; i++)
		{
			if (root.contains((*bodies)[i].getPos()))
			{
				p_tree->updateAccel((*bodies)[i]);
				(*bodies)[i].update(TIMESTEP);
				(*bodies)[i].resetAccel();
			}
		}
	}
}

std::wstring charToWstring(const char* str)
{
	const size_t size = strlen(str);
	std::wstring wstr;
	if (size > 0) {
		wstr.resize(size);
		mbstowcs(&wstr[0], str, size);
	}
	return wstr;
}

using namespace nbody;

int main2()
{
	Sim sim;

	sim.pushState(new StartState(&sim));
	sim.simLoop();

	return 0;
}

int main()
{
	//AllocConsole();
	//AttachConsole(GetCurrentProcessId());
	//freopen("CON", "w", stdout);

	try
	{
		//mainBH();
		main2();
	}
	catch (Error e)
	{
		MessageBox(NULL, charToWstring(e.what()).data(), NULL, MB_ICONERROR);
		return 1;
	}
	/*catch (...)
	{
		std::cout << "Something happened...";
		return 1;
	}*/
	return 0;
}
