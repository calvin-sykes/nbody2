#ifndef SIM_H
#define SIM_H

#include "AssetManager.h"
#include "Body2d.h"
#include "BodyGroupProperties.h"
#include "Evolver.h"
#include "Integrator.h"

#include <SFML/Graphics.hpp>

#include <iterator>
#include <stack>
#include <vector>

namespace nbody
{
	struct BodyGroupProperties;
	class SimState;

	// Apend a vector rvalue to a secoond vector, using move semantics
	template <typename T>
	typename std::vector<T>::iterator append(std::vector<T>&& src, std::vector<T>& dest)
	{
		typename std::vector<T>::iterator result;

		if (dest.empty())
		{
			dest = std::move(src);
			result = std::begin(dest);
		}
		else
		{
			result = dest.insert(std::end(dest),
				std::make_move_iterator(std::begin(src)),
				std::make_move_iterator(std::end(src)));
		}

		src.clear();
		src.shrink_to_fit();

		return result;
	}


	struct SimProperties
	{
		SimProperties() : int_type(IntegratorType::INVALID), ev_type(EvolverType::INVALID), bg_props{} {}
		
		IntegratorType int_type;
		EvolverType ev_type;
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
		void loadObjects();

		void setProperties(SimProperties const&);
		void createBodyGroup(BodyGroupProperties const&);
		void simLoop();
		
		Integrator * integrator_ptr;
		Evolver * evolver_ptr;

		std::vector<Body2d> bodies;
		
		sf::RenderWindow window;
		AssetManager asset_mgr;
		sf::Sprite background;

	private:
		std::stack<SimState*> states;
	};
}
#endif // SIM_H