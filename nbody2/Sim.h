#ifndef SIM_H
#define SIM_H

#include "AssetManager.h"

#include "imgui.h"
#include "imgui_sfml.h"

#include <stack>

#include <SFML/Graphics.hpp>


namespace nbody
{
	class SimState;

	class Sim
	{
	public:
		Sim();
		~Sim();

		void pushState(SimState* state);
		void popState();
		void changeState(SimState* state);
		SimState* peekState();
		
		void loadTextures();

		void simLoop();
		
		sf::RenderWindow window;
		AssetManager asset_mgr;
		sf::Sprite background;

	private:
		std::stack<SimState*> states;

		unsigned char* pixels;
		int width, height;

		
	};
}
#endif // SIM_H