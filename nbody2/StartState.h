#ifndef START_STATE_H
#define START_STATE_H

#include "SimState.h"

#include <SFML/Graphics.hpp>

namespace nbody
{
	enum class MenuState
	{
		INITIAL,
		LOAD_EXISTING,
		CREATE_NEW
	};

	class StartState : public SimState
	{
	public:
		StartState(Sim * simIn);

		virtual void draw(sf::Time const dt);
		virtual void update(sf::Time const dt);
		virtual void handleInput();
	private:
		void run();

		sf::View view;

		MenuState menu_state;
		bool modal_is_open;
		ImGuiWindowFlags window_flags;
		ImGuiStyle& style;

	};
}

#endif // START_STATE_H
