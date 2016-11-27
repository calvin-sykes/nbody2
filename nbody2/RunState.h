#ifndef RUN_STATE_H
#define RUN_STATE_H

#include "SimState.h"

#include <SFML/Graphics.hpp>

namespace nbody
{
	enum class ActionState { NONE, PANNING };

	class RunState : public SimState
	{
	private:
		sf::View main_view;
		sf::View gui_view;

		ActionState action_state;
		float zoom_level;

	public:
		virtual void draw(sf::Time const dt);
		virtual void update(sf::Time const dt);
		virtual void handleInput();

		RunState(Sim * game);
	};
}

#endif // RUN_STATE_H
