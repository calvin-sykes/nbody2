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
	}

	void RunState::draw(sf::Time const dt)
	{
		this->sim->window.clear(sf::Color::Black);
		this->sim->window.draw(this->sim->background);
	}

	void RunState::update(sf::Time const dt)
	{
		// empty
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