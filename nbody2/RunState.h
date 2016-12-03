#ifndef RUN_STATE_H
#define RUN_STATE_H

#include "SimState.h"
#include "Vector.h"

#include <SFML/Graphics.hpp>

namespace nbody
{
	class BHTree;
	class Quad;
	class Sim;

	enum class ActionState { NONE, PANNING };

	class RunState : public SimState
	{
		friend class BarnesHutEvolver;

	private:
		sf::View main_view;
		sf::View gui_view;

		ActionState action_state;

		Vector2d com;
		BHTree * tree_ptr;

		bool running = false;
		bool show_bodies = true;
		bool show_grid = false;
		bool show_grid_levels = false;
		bool show_trails = false;
		bool current_show_grid = false;
		bool view_centre = true;
		bool view_dragging = false;
		bool tree_old = true;

	public:
		virtual void draw(sf::Time const dt);
		virtual void update(sf::Time const dt);
		virtual void handleInput();

		RunState(Sim * game);
	};
}

#endif // RUN_STATE_H
