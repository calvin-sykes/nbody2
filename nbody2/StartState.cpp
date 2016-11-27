#include "StartState.h"
#include "RunState.h"
#include "SimState.h"

#include <SFML/Graphics.hpp>

namespace ImGui
{
	bool CentredButton(char const* label, ImVec2 const& size)
	{
		auto initial_pos = GetCursorPosX();
		auto res = Button("", size);
		SameLine();
		SetCursorPosX(initial_pos);
		PushTextWrapPos(size.x + initial_pos);
		TextWrapped(label);
		PopTextWrapPos();
		return res;
	}
}

namespace nbody
{
	StartState::StartState(Sim * simIn) : style(ImGui::GetStyle())
	{
		this->sim = simIn;
		sf::Vector2f pos = sf::Vector2f(this->sim->window.getSize());
		this->view.setSize(pos);
		this->view.setCenter(0.5f * pos);

		// Set layout flags for the GUI windows
		this->window_flags = 0;
		this->window_flags |= ImGuiWindowFlags_NoCollapse;
		this->window_flags |= ImGuiWindowFlags_NoResize;
		this->window_flags |= ImGuiWindowFlags_NoMove;
		this->window_flags |= ImGuiWindowFlags_NoScrollbar;

		// Set some styles for the GUI windows
		this->style.ButtonTextAlign = { 0.5f, 0.5f };
		this->style.WindowTitleAlign = { 0.5f, 0.5f };

		// Start at the initial menu state 
		this->menu_state = MenuState::INITIAL;
	}

	void StartState::draw(sf::Time const dt)
	{
		this->sim->window.setView(this->view);
		this->sim->window.clear(sf::Color::Black);
		this->sim->window.draw(this->sim->background);

		this->sim->window.pushGLStates();
		this->sim->window.resetGLStates();
		ImGui::Render();
		this->sim->window.popGLStates();
	}

	void StartState::update(sf::Time const dt)
	{
		using namespace ImGui;

		SFML::Update(this->sim->window, dt);

		if (this->menu_state == MenuState::INITIAL)
		{
			// Centre the window on screen
			SetNextWindowPosCenter();
			// Set window's size
			SetNextWindowSize(ImVec2{ 500, 300 });

			Begin("N-body Simulator", 0, window_flags);
			Dummy({ GetContentRegionAvailWidth(), 50 });
			BeginGroup();
			if(Button("Load existing\ninitial conditions", { GetContentRegionAvailWidth() * 0.5f, 50 }))
			{
				this->menu_state = MenuState::LOAD_EXISTING;
			}
			SameLine();
			if (CentredButton("Generate new\ninitial conditions", { GetContentRegionAvailWidth(), 50 }))
			{
				this->menu_state = MenuState::CREATE_NEW;
			}
			EndGroup();
			Dummy({ GetContentRegionAvailWidth(), 50 });
			Dummy({ GetContentRegionAvailWidth() / 3, 50 });
			SameLine();
			if (Button("Quit", { GetContentRegionAvailWidth() * 0.5f, 50 }))
			{
				this->sim->window.close();
			}
			End();
		}
	}

	void StartState::handleInput()
	{
		sf::Event event;

		while (this->sim->window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			switch (event.type)
			{
			case sf::Event::Closed:
			{
				this->sim->window.close();
				break;
			}
			case sf::Event::Resized:
			{
				this->view.setSize(event.size.width, event.size.height);
				this->sim->background.setPosition(this->sim->window.mapPixelToCoords(sf::Vector2i(0, 0), this->view));
				this->sim->background.setScale(
					float(event.size.width) / float(this->sim->background.getTexture()->getSize().x),
					float(event.size.height) / float(this->sim->background.getTexture()->getSize().y));
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape)
					this->sim->window.close();
				else if (event.key.code == sf::Keyboard::Space)
					//this->run();
					break;
			}
			default:
				break;
			}
		}
	}

	void StartState::run()
	{
		this->sim->pushState(new RunState(this->sim));
	}
}