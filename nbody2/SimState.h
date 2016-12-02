#ifndef SIM_STATE_H
#define SIM_STATE_H

#include <SFML/System/Clock.hpp>

namespace nbody
{
	class Sim;
	
	class SimState
	{
	public:

		Sim* sim;

		virtual void draw(sf::Time const  dt) = 0;
		virtual void update(sf::Time const dt) = 0;
		virtual void handleInput() = 0;
	};
}
#endif // SIM_STATE_H