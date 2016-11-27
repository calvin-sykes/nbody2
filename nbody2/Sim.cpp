#include <stack>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Sim.h"
#include "SimState.h"

#define WINDOW_W 800
#define WINDOW_H 600

namespace nbody
{
	Sim::Sim()
	{
		// Load images from disk
		this->loadTextures();
		// Create SFML window
#ifndef NDEBUG
		this->window.create(sf::VideoMode(WINDOW_W, WINDOW_H), "nbody2");
#else
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		auto mode = sf::VideoMode().getFullscreenModes()[0];
		this->window.create(mode, "nbody2", sf::Style::Fullscreen, settings);
#endif
		this->window.setVerticalSyncEnabled(true);
		this->window.setFramerateLimit(120);
		// Set background texture
		this->background.setTexture(this->asset_mgr.getTextureRef("background"));
		this->background.setScale(
			float(window.getSize().x) / float(this->background.getTexture()->getSize().x),
			float(window.getSize().y) / float(this->background.getTexture()->getSize().y));
		// Set window icon
		auto icon_image = this->asset_mgr.getTextureRef("icon").copyToImage();
		this->window.setIcon(icon_image.getSize().x, icon_image.getSize().y, icon_image.getPixelsPtr());
		// Initialise GUI
		ImGui::SFML::Init(window);

		// Load font
/*		ImGuiIO & io = ImGui::GetIO();

		io.Fonts->AddFontFromFileTTF("media/segoeui.ttf", 12);
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		sf::Texture font_tex;
		font_tex.loadFromMemory(pixels, width * height);
		ImGui::SFML::setFontTexture(font_tex);
*/
	}

	Sim::~Sim()
	{
		while (!(this->states.empty())) popState();
		// Clean up GUI
		ImGui::SFML::Shutdown();
	}

	void Sim::pushState(SimState* state)
	{
		this->states.push(state);
	}

	void Sim::popState()
	{
		delete this->states.top();
		this->states.pop();
	}

	void Sim::changeState(SimState* state)
	{
		if (!this->states.empty())
			popState();
		pushState(state);
	}

	SimState* Sim::peekState()
	{
		if (this->states.empty()) return nullptr;
		else return this->states.top();
	}

	void Sim::loadTextures()
	{
		asset_mgr.loadTexture("background", "media/background.png");
		asset_mgr.loadTexture("icon", "media/sfml.png");
	}

	void Sim::simLoop()
	{
		sf::Clock clock;

		while (this->window.isOpen())
		{
			auto dt = clock.restart();

			if (peekState() == nullptr) continue;
			peekState()->handleInput();
			peekState()->update(dt);
			this->window.clear(sf::Color::Black);
			peekState()->draw(dt);
			this->window.display();
		}
	}
}