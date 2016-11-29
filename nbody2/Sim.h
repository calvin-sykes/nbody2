#ifndef SIM_H
#define SIM_H

#include "Body2d.h"

#include "AssetManager.h"
#include "imgui.h"
#include "imgui_sfml.h"

#include <stack>
#include <map>
#include <vector>

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

		double static constexpr SOLAR_MASS = 1.98892E30;
		double static constexpr RADIUS = 1E18;
		size_t static constexpr MAX_N = 10000;
		
		sf::RenderWindow window;
		AssetManager asset_mgr;
		sf::Sprite background;

	private:
		std::stack<SimState*> states;

		std::vector<Body2d> bodies;
	};
}
#endif // SIM_H