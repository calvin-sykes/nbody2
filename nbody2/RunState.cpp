#include "BHTree.h"
#include "Constants.h"
#include "Display.h"
#include "Quad.h"
#include "RunState.h"
#include "Sim.h"
#include "SimState.h"

#include <SFML/Graphics.hpp>

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

		Body2d::integrator_ptr = this->sim->integrator_ptr;
	}

	BHTree * RunState::buildTreeThreaded(size_t const n, std::vector<Body2d> const& bodies, Quad const & root)
	{
		BHTree * nwt, * net, * swt, * set;
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

	BHTree * RunState::buildTree(size_t const n, std::vector<Body2d> const & bodies, Quad const & root)
	{
		auto tree_ptr = new BHTree(root, 0);
		for (auto& b : bodies)
			tree_ptr->insert(&b);
		return tree_ptr;
	}

	void RunState::stepBH(size_t const n, std::vector<Body2d> & bodies, BHTree const * tree_ptr, Quad const & root)
	{
#pragma omp parallel for schedule(static)
		for (int i = 0; i < n; i++)
		{
			if (root.contains(bodies[i].getPos()))
			{
				tree_ptr->updateAccel(bodies[i]);
				bodies[i].update(TIMESTEP);
				bodies[i].resetAccel();
			}
		}
	}

	void RunState::draw(sf::Time const dt)
	{
		this->sim->window.clear(sf::Color::Black);
		
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

		if (show_grid)
		{
			tree_ptr->updateGfx(show_grid_levels);
			this->sim->window.draw(*tree_ptr);
		}

		//window.display();

		// finished with tree; it is regenerated from scratch next time
		// delete to prevent memory leak
		if (running || tree_old)
		{
			delete tree_ptr;
		}
	}

	void RunState::update(sf::Time const dt)
	{
		// create root quadrant of tree
		Quad root(com.x, com.y, 10 * Constants::RADIUS);

		// reconstruct BH tree if needed
		if (tree_old || current_show_grid != show_grid)
		{
			tree_ptr = buildTreeThreaded(N, this->sim->bodies, root);
			com = tree_ptr->getPCoM();
			tree_old = false;
			current_show_grid = show_grid;
		}

		if (running)
		{
			// advance bodies
			stepBH(N, this->sim->bodies, tree_ptr, root);
			tree_old = true;
		}
	}

	void RunState::handleInput()
	{
		sf::Event event;

		while (this->sim->window.pollEvent(event))
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
				this->main_view.setSize(event.size.width, event.size.height);
				this->gui_view.setSize(event.size.width, event.size.height);
				this->sim->background.setPosition(
					this->sim->window.mapPixelToCoords(sf::Vector2i(0, 0), this->gui_view));
				this->sim->background.setScale(
					float(event.size.width) / float(this->sim->background.getTexture()->getSize().x),
					float(event.size.height) / float(this->sim->background.getTexture()->getSize().y));
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape)
					this->sim->window.close();
				break;
			}
			default:
				break;
			}
		}
	}
}