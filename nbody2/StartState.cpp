#include "StartState.h"
#include "RunState.h"
#include "SimState.h"

#include <iostream>

#include <SFML/Graphics.hpp>

namespace ImGui
{
	bool CentredButton(char const* label, ImVec2 const& size)
	{
		// Count number of carriage returns //
		size_t n_lines = 0;
		auto len = strlen(label);
		auto lines = new char*[len + 1]; // Worst case: every character is carriage return
		/*// Beginning of this line
		size_t beg_pos = 0, i = 0;
		do //for (size_t i = 0; *(label + i) != '\0'; i++)
		{
			// copy when newline is found, or when end is reached
			if (*(label + i) == '\n' || i == len)
			{
				// copy this line into array
				lines[n_lines] = new char[i - beg_pos + 1]; // +1 is for null terminator
				strncpy(lines[n_lines], label + beg_pos, i - beg_pos + 1);
				// null-terminate
				lines[n_lines][i - beg_pos] = '\0';
				// next line starts one character later
				beg_pos = i + 1;
				n_lines++;
			}
		} while (*(label + i) != '\0' && ++i);
		*/
		auto mod_label = new char[len + 1];
		strcpy(mod_label, label);
		auto pch = strtok(mod_label, "\n");

		while (pch != nullptr)
		{
			lines[n_lines] = new char[strlen(pch) + 1];
			strcpy(lines[n_lines++], pch);
			pch = strtok(nullptr, "\n");
		}
		
		// figure out length of each line and find the maximum length
		size_t* line_lens = new size_t[n_lines];
		size_t max_len = 0;
		for (size_t i = 0; i < n_lines; i++)
		{
			line_lens[i] = strlen(lines[i]);
			if (line_lens[i] > max_len)
				max_len = line_lens[i];
		}

		// length needed = longest line * number of lines + extra for newlines / null terminator
		char* formatted = new char[(max_len + 1) * n_lines];
		// copy strings, padding with spaces either side
		for (size_t i = 0; i < n_lines; i++)
		{
			size_t pos = i * (max_len + 1);
			// num of chars to fill with spaces
			auto extras = max_len - line_lens[i];
			// before text
			memset((void*)(formatted + pos), ' ', extras / 2);
			// text
			pos += extras / 2;
			strncpy(formatted + pos, lines[i], line_lens[i]);
			// after text, overwriting newline / null terminator
			pos += line_lens[i];
			memset((void*)(formatted + pos), ' ', max_len - (pos - (max_len + 1) * i));
			pos += max_len - (pos - (max_len + 1) * i);
			// end line
			formatted[pos] = (i != n_lines - 1) ? '\n' : '\0';
		}

		// clean up
		delete[] mod_label;
		for (size_t i = 0; i < n_lines; i++)
		{
			delete[] lines[i];
		};
		delete[] line_lens;
		delete[] lines;

		// display button before deleting the formatted label
		auto res = Button(formatted, size);
		delete[] formatted;
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

		ImGui::Render();
		this->sim->window.resetGLStates();
	}

	void StartState::update(sf::Time const dt)
	{
		using namespace ImGui;

		SFML::Update(this->sim->window, dt);

		if (this->menu_state == MenuState::INITIAL)
		{
			SetNextWindowPosCenter();
			SetNextWindowSize(ImVec2{ 500, 300 });
			Begin("N-body Simulator", 0, this->window_flags);
			Dummy({ GetContentRegionAvailWidth(), 50 });
			BeginGroup();
			if (CentredButton("Load existing\ninitial conditions", { GetContentRegionAvailWidth() * 0.5f, 50 }))
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
		else if (this->menu_state == MenuState::CREATE_NEW)
		{
			SetNextWindowPosCenter();
			SetNextWindowSize(ImVec2{ 700, 500 });
			
			bool persist = true;

			Begin("N-body Simulator | Generate initial conditions", &persist, this->window_flags);
			Text("Do stuff here.");
			End();

			if (!persist)
				this->menu_state = MenuState::INITIAL;
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