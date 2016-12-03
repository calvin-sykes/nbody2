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

	struct Flags
	{
		Flags() :
			running(false), show_bodies(true), show_trails(false),
			show_grid(false), show_grid_levels(false), current_show_grid(false),
			view_centre(true), view_dragging(false),
			tree_old(true), tree_exists(false) {}

		bool running : 1;
		bool show_bodies : 1;
		bool show_trails : 1;
		bool show_grid : 1;
		bool show_grid_levels : 1;
		bool current_show_grid : 1;

		bool view_centre : 1;
		bool view_dragging : 1;
		bool tree_old : 1;
		bool tree_exists : 1;
	};

	class RunState : public SimState
	{
	private:
		sf::View main_view;
		sf::View gui_view;

		Vector2d com;
		BHTree * tree_ptr;

		Flags flags;

	public:
		virtual void draw(sf::Time const dt);
		virtual void update(sf::Time const dt);
		virtual void handleInput();

		RunState(Sim * game);
	};
}

#endif // RUN_STATE_H
