#ifndef SIM_H
#define SIM_H

#include "Body2d.h"
#include "BodyGroupProperties.h"
#include "Integrator.h"

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

	// TODO: Move me?
	enum class EvolveType
	{
		BRUTE_FORCE,
		BARNES_HUT,
		N_METHODS,
		INVALID = -1
	};
	// also TODO
	struct EvolveProperties
	{
		EvolveType type;
		char const* name;
		char const* tooltip;
	};

	struct SimProperties
	{
		SimProperties() : int_type(IntegratorType::INVALID), ev_type(EvolveType::INVALID), bg_props{} {}
		
		IntegratorType int_type;
		EvolveType ev_type;
		std::vector<BodyGroupProperties> bg_props;
	};

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
		
		Integrator * integrator;
		
		sf::RenderWindow window;
		AssetManager asset_mgr;
		sf::Sprite background;

	private:
		std::stack<SimState*> states;

		std::vector<Body2d> bodies;
	};
}
#endif // SIM_H