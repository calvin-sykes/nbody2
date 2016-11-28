#include "StartState.h"
#include "RunState.h"
#include "SimState.h"

#include <stdarg.h>

#include <iostream>

#include <SFML/Graphics.hpp>

namespace ImGui
{
	static void ShowHelpMarker(char const* fmt, ...)
	{
		TextDisabled("(?)");
		if (IsItemHovered())
		{
			BeginTooltip();
			PushTextWrapPos(200.0f);
			va_list args;
			va_start(args, fmt);
			TextWrappedV(fmt, args);
			va_end(args);
			PopTextWrapPos();
			EndTooltip();
		}
	}

	bool CentredButton(char const* label, ImVec2 const& size)
	{
		size_t n_lines = 0;
		auto len = strlen(label);
		auto lines = new char*[len + 1]; // Worst case: every character is carriage return

		// Make a modifiable copy of the argument
		auto label_cpy = new char[len + 1];
		// Split the argument acroos newline characters
		char * next_tok = nullptr;
		strcpy_s(label_cpy, len + 1, label);
		auto tok = strtok_s(label_cpy, "\n", &next_tok);
		while (tok != nullptr)
		{
			lines[n_lines] = new char[strlen(tok) + 1];
			strcpy_s(lines[n_lines++], strlen(tok) + 1, tok);
			tok = strtok_s(nullptr, "\n", &next_tok);
		}

		// Figure out length of each line and find the maximum length
		size_t* line_lens = new size_t[n_lines];
		size_t max_len = 0;
		for (size_t i = 0; i < n_lines; i++)
		{
			line_lens[i] = strlen(lines[i]);
			if (line_lens[i] > max_len)
				max_len = line_lens[i];
		}

		// Length of formatted string
		//	= number of lines  *( longest line * + 1 for newlines / null terminator)
		auto formatted_len = (max_len + 1) * n_lines;
		char* formatted = new char[formatted_len];
		// copy strings, padding with spaces either side
		for (size_t i = 0; i < n_lines; i++)
		{
			size_t pos = i * (max_len + 1);
			// num of chars to fill with spaces
			auto extras = max_len - line_lens[i];
			// before text
			memset((void*)(formatted + pos), ' ', extras / 2);
			pos += extras / 2;
			// text
			strncpy_s(formatted + pos, formatted_len - pos, lines[i], line_lens[i]);
			pos += line_lens[i];
			// after text until line is full, overwriting newline / null terminator
			auto remaining_len = max_len - (pos - (max_len + 1) * i);
			memset((void*)(formatted + pos), ' ', remaining_len);
			pos += remaining_len;
			// replace newline / null terminator
			formatted[pos] = (i != n_lines - 1) ? '\n' : '\0';
		}

		// clean up
		delete[] label_cpy;
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
	// Initialise array of BodyDistributor information

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

		// Centre-align the GUI window titles
		this->style.WindowTitleAlign = { 0.5f, 0.5f };

		// Initialise empty container for BodyGroupProperties
		this->bg_props = {};

		// Start at the initial menu, which prompts to load or create
		// a set of initial conditions
		this->menu_state = MenuState::INITIAL;
	}

	void StartState::draw(sf::Time const dt)
	{
		this->sim->window.resetGLStates();
		this->sim->window.setView(this->view);
		this->sim->window.clear(sf::Color::Black);
		this->sim->window.draw(this->sim->background);
		ImGui::Render();
	}

	void StartState::update(sf::Time const dt)
	{
		using namespace ImGui;

		SFML::Update(this->sim->window, dt);

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

		if (this->menu_state == MenuState::CREATE_NEW)
		{
			l1_modal_is_open = true;
			SetNextWindowSize({ 700,500 });
			SetNextWindowPosCenter();
			OpenPopup("Generate initial conditions");
			if (BeginPopupModal("Generate initial conditions", &l1_modal_is_open, window_flags))
			{
				// Main display of BodyGroup properties
				BeginChild("groups", { 0, 400 });
				// Show prompt if no BodyGroups have been added
				if (bg_props.size() == 0)
				{
					Text("Click \"+\" to add a group of bodies.");
				}
				// Groups are bounded by square-cornered rectangles
				PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 0.f);
				for (size_t i = 0; i < this->bg_props.size(); i++)
				{
					// Create unique ID for group
					char idx[4] = {}; // allows up to 999 groups
					sprintf_s<4>(idx, "%zu", i);
					char entry_id[16] = {};
					strcat_s<16>(entry_id, "group##");
					strcat_s<16>(entry_id, idx);

					// Single set of BodyGroup properties //
					BeginChild(entry_id, { 0, 100 }, true);
					// Number label
					Text("Group %u", i + 1);
					// Number of bodies slider
					SameLine();
					PushItemWidth(0.5f * GetContentRegionAvailWidth());
					SliderInt("Number of bodies", &(this->bg_props[i].N), 0, N_MAX);
					PopItemWidth();
					// Distributor combobox
					SameLine();
					PushItemWidth(GetContentRegionAvailWidth() - CalcTextSize("Distribution").x);
					auto sel_dist = (int*)(&bg_props[i].dist);
					if (Combo("Distribution", sel_dist, getDistributorName,
						(void*)(this->dist_infos.data()), this->dist_infos.size()))
					{
						this->bg_props[i].dist = this->dist_infos[*sel_dist].type;
						this->bg_props[i].has_central_mass =
							this->dist_infos[*sel_dist].has_central_mass;
					}
					PopItemWidth();
					if (IsItemHovered() && *sel_dist != -1)
					{
						BeginTooltip();
						Text(this->dist_infos[*sel_dist].tooltip);
						EndTooltip();
					}
					Dummy({ 0, 5 });
					// Mass range popup
					if (Button("Mass...", { 0.25f * GetContentRegionAvailWidth(), 0 }))
					{
						l2_modal_is_open = true;
						SetNextWindowPosCenter();
						SetNextWindowSize({ 0, 0 });
						OpenPopup("Mass settings");
					}
					if (BeginPopupModal("Mass settings", &l2_modal_is_open, window_flags))
					{
						AlignFirstTextHeightToWidgets();
						Text("Minimum");
						SameLine();
						PushItemWidth(0.5f * GetContentRegionAvailWidth());
						InputFloat("solar masses##1", &this->tmp_min_mass[i]);
						PopItemWidth();
						AlignFirstTextHeightToWidgets();
						Text("Maximum");
						SameLine();
						PushItemWidth(0.5f * GetContentRegionAvailWidth());
						InputFloat("solar masses##2", &this->tmp_max_mass[i]);
						PopItemWidth();
						// disallow negative values
						if (tmp_min_mass[i] < 0)
							tmp_min_mass[i] *= -1;
						if (tmp_max_mass[i] < 0)
							tmp_max_mass[i] *= -1;
						Dummy({ 100, 0 });
						SameLine();
						if (Button("OK"))
						{
							this->bg_props[i].min_mass = this->tmp_min_mass[i] * this->sim->SOLAR_MASS;
							this->bg_props[i].max_mass = this->tmp_max_mass[i] * this->sim->SOLAR_MASS;
							CloseCurrentPopup();
						}
						SameLine();
						Dummy({ 100, 0 });
						EndPopup();
					}
					// Central mass
					SameLine();
					if (Button("Central mass...", { 0.33f * GetContentRegionAvailWidth(), 0 }))
					{
						l2_modal_is_open = true;
						SetNextWindowPosCenter();
						SetNextWindowSize({ 0,0 });
						OpenPopup("Central mass setting");
					}
					if (BeginPopupModal("Central mass setting", &l2_modal_is_open, window_flags))
					{
						char const* label = "1E6 solar masses";
						PushItemWidth(GetContentRegionAvailWidth() - CalcTextSize(label).x);
						InputFloat(label, &tmp_central_mass[i]);
						// disallow negative values
						if (tmp_central_mass[i] < 0)
							tmp_central_mass[i] *= -1;
						Dummy({ 100, 0 });
						SameLine();
						if (Button("OK"))
						{
							this->bg_props[i].central_mass = this->tmp_central_mass[i] * this->sim->SOLAR_MASS * 1E6;
							CloseCurrentPopup();
						}
						SameLine();
						Dummy({ 100, 0 });
						EndPopup();
					}
					// Position and velocity popup
					SameLine();
					if (Button("Position/velocity...", { 0.5f * GetContentRegionAvailWidth(), 0 }))
					{
						l2_modal_is_open = true;
						SetNextWindowPosCenter();
						SetNextWindowSize({ 0,0 });
						OpenPopup("Position/velocity settings");
					}
					if (BeginPopupModal("Position/velocity settings", &l2_modal_is_open, window_flags))
					{
						Checkbox("Use relative coordinates", (bool*)&this->tmp_use_relative_coords[i]);
						SameLine();
						ShowHelpMarker("If checked, positions and velocities entered will be scaled by the universe radius %.0e",
							this->sim->RADIUS);
						PushItemWidth(GetContentRegionAvailWidth() - CalcTextSize("        ").x);
						InputFloat2("Position", (float*)&this->tmp_pos[i].x);
						InputFloat2("Velocity", (float*)&this->tmp_vel[i].x);
						PopItemWidth();
						Dummy({ 100,0 });
						SameLine();
						if (Button("OK"))
						{
							this->bg_props[i].pos = this->tmp_pos[i];
							this->bg_props[i].vel = this->tmp_vel[i];
							if (this->tmp_use_relative_coords[i])
							{
								this->bg_props[i].pos *= this->sim->RADIUS;
								this->bg_props[i].vel *= this->sim->RADIUS;
							}
							CloseCurrentPopup();
						}
						SameLine();
						Dummy({ 100,0 });
						EndPopup();
					}

					EndChild();
				}
				PopStyleVar();
				EndChild();

				Separator();
				SameLine();
				Dummy({ GetContentRegionAvailWidth() - 30, 0 });
				SameLine();

				// Add/remove buttons
				BeginGroup();
				PushItemWidth(-1);
				if (Button("+"))
				{
					this->bg_props.emplace_back(BodyGroupProperties());
					this->tmp_dist_type.push_back(DistributorType::INVALID);
					this->tmp_max_mass.push_back(0);
					this->tmp_min_mass.push_back(0);
					this->tmp_central_mass.push_back(0);
					this->tmp_pos.emplace_back(Vector2d());
					this->tmp_vel.emplace_back(Vector2d());
					this->tmp_use_relative_coords.push_back(1);
				}
				if (Button("-") && bg_props.size() != 0)
				{
					this->bg_props.pop_back();
					this->tmp_dist_type.pop_back();
					this->tmp_max_mass.pop_back();
					this->tmp_min_mass.pop_back();
					this->tmp_central_mass.pop_back();
					this->tmp_pos.pop_back();
					this->tmp_vel.pop_back();
					this->tmp_use_relative_coords.pop_back();
				}
				PopItemWidth();
				EndGroup();

				EndPopup();
			}
		}
		if (!l1_modal_is_open)
		{
			menu_state = MenuState::INITIAL;
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