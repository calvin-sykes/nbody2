#include "BodyGroupProperties.h"
#include "Display.h"
#include "Sim.h"
#include "SimState.h"

#include "imgui.h"
#include "imgui_sfml.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#define WINDOW_W 800
#define WINDOW_H 600

namespace nbody
{
	Sim::Sim()
	{
		// Load images from disk
		this->loadTextures();
		// Initialise objects
		this->loadObjects();
		// Load font
		unsigned char* pixels;
		int width;
		int height, bytes_per_pixel;
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("media/DroidSans.ttf", 14.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
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
		// Initialise values needed for world-screen coordinate conversion
		Display::screen_size = this->window.getSize();
		Display::aspect_ratio = static_cast<float>(Display::screen_size.x) / static_cast<float>(Display::screen_size.y);
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
		this->asset_mgr.loadTexture("background", "media/background.png");
		this->asset_mgr.loadTexture("icon", "media/sfml.png");
	}

	void Sim::loadObjects()
	{
		this->asset_mgr.loadIntegrators();
		this->asset_mgr.loadEvolvers();
		this->asset_mgr.loadDistributors();
	}

	void Sim::loadFonts()
	{
		this->asset_mgr.loadFont("droid_sans", "media/DroidSans.ttf");
	}

	void Sim::setProperties(SimProperties const& props)
	{
		this->integrator_ptr = asset_mgr.getIntegrator(props.int_type);
		this->evolver_ptr = asset_mgr.getEvolver(props.ev_type);
		for (auto& bgp : props.bg_props)
		{
			createBodyGroup(bgp);
		}
	}

	void Sim::createBodyGroup(BodyGroupProperties const& bgp)
	{
		auto* distrib = asset_mgr.getDistributor(bgp.dist);
		append(distrib->createDistribution(bgp), bodies);
	}

	void Sim::simLoop()
	{
		sf::Clock clock;

		while (this->window.isOpen())
		{
			auto dt = clock.restart();

			auto current_state = peekState();
			if (current_state == nullptr) continue;
			current_state->handleInput();
			current_state->update(dt);
			this->window.clear(sf::Color::Black);
			current_state->draw(dt);
			this->window.display();
		}
	}
}