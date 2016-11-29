#include "StartState.h"
#include "RunState.h"
#include "SimState.h"

#include <stdarg.h>
#include <numeric>

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
		ImGui::SFML::Update(this->sim->window, dt);

		InitialWindow();

		if (this->menu_state == MenuState::CREATE_NEW)
		{
			l1_modal_is_open = true;
			GenerateWindow();
		}
		else if (this->menu_state == MenuState::LOAD_EXISTING)
		{
			l1_modal_is_open = true;
			LoadWindow();
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

	void StartState::InitialWindow()
	{
		using namespace ImGui;

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

	void StartState::LoadWindow()
	{

	}
	void StartState::GenerateWindow()
	{
		using namespace ImGui;

		SetNextWindowSize({ 700,500 });
		SetNextWindowPosCenter();
		OpenPopup("Generate initial conditions");
		if (BeginPopupModal("Generate initial conditions", &l1_modal_is_open, window_flags))
		{
			// Main display of BodyGroup properties
			BeginChild("groups", { 0, 400 });
			// Show prompt if no BodyGroups have been added
			if (this->bg_props.size() == 0)
			{
				Text("Click \"+\" to add a group of bodies.");
			}
			float sz;
			if(this->bg_props.size() > 0)
				sz = max(static_cast<float>((400 - 2 * style.FramePadding.y - (this->bg_props.size() - 1) * style.ItemSpacing.y)/ this->bg_props.size()), 70.f);
			// Groups are bounded by square-cornered rectangles
			PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 0.f);
			for (size_t i = 0; i < this->bg_props.size(); i++)
			{
				// Create unique ID for group
				char entry_id[16] = {};
				sprintf_s<16>(entry_id, "group##%zu", i);
				// Single set of BodyGroup properties //
				BeginChild(entry_id, { 0, sz }, true);
				// Number label
				Text("Group %zu", i + 1);
				// Number of bodies slider
				SameLine();
				PushItemWidth(0.5f * GetContentRegionAvailWidth());
				SliderInt("Number of bodies", &(this->bg_props[i].N), 0, this->sim->MAX_N);
				PopItemWidth();
				auto n_total = std::accumulate(bg_props.cbegin(), bg_props.cend(), 0, [](int sum, BodyGroupProperties const& b) -> int { return sum + b.N; });
				// too many bodies, need to redistribute them
				if (n_total > this->sim->MAX_N)
				{
					// if too many, steal from previous
					// except if first, then steal from last
					// if more need to be stolen than are available, move to next
					auto n_excess = n_total - static_cast<int>(this->sim->MAX_N);
					size_t j = 1;
					while (n_excess > 0)
					{
						// how many are available to steal?
						// take as reference so we can modify it
						auto& subtractable = this->bg_props[(i + j) % (this->bg_props.size())].N;
						if (n_excess < subtractable)
						{
							// can steal required from this
							subtractable -= n_excess;
							// done
							n_excess = 0;
						}
						else
						{
							// take what we can
							n_excess -= subtractable;
							subtractable = 0;
							// move on
							j++;
						}
					}

				}
				// Distributor combobox
				SameLine();
				PushItemWidth(GetContentRegionAvailWidth() - CalcTextSize("Distribution").x);
				auto sel_dist = (int*)(&bg_props[i].dist);
				if (Combo("Distribution", sel_dist, getDistributorName,
					(void*)(this->dist_infos.data()), static_cast<int>(this->dist_infos.size())))
				{
					//this->bg_props[i].dist = this->dist_infos[*sel_dist].type;
					this->bg_props[i].has_central_mass =
						this->dist_infos[*sel_dist].has_central_mass;
				}
				PopItemWidth();
				if (IsItemHovered() && *sel_dist != -1)
				{
					BeginTooltip();
					PushTextWrapPos(200);
					TextWrapped(this->dist_infos[*sel_dist].tooltip);
					PopTextWrapPos();
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
					MassPopup(i);
					EndPopup();
				}
				// Central mass
				SameLine();
				if (Button("Central mass...", { 0.33f * GetContentRegionAvailWidth(), 0 }))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					SetNextWindowSize({ 0, 0 });
					OpenPopup("Central mass setting");
				}
				if (BeginPopupModal("Central mass setting", &l2_modal_is_open, window_flags))
				{
					CentralMassPopup(i);
					EndPopup();
				}
				// Position and velocity popup
				SameLine();
				if (Button("Position/velocity...", { 0.5f * GetContentRegionAvailWidth(), 0 }))
				{
					l2_modal_is_open = true;
					SetNextWindowPos({ 400, 400 });
					SetNextWindowSize({ 0, 0 });
					OpenPopup("Position/velocity settings");
				}
				if (BeginPopupModal("Position/velocity settings", &l2_modal_is_open, window_flags))
				{
					PosVelPopup(i);
					EndPopup();
				}
				// Colour settings popup
				SameLine();
				if (Button("Colour...", { GetContentRegionAvailWidth(), 0 }))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					SetNextWindowSize({ 250, 150 });
					OpenPopup("Colour settings");
				}
				if (BeginPopupModal("Colour settings", &l2_modal_is_open, window_flags))
				{
					ColourPopup(i);
					EndPopup();
				}

				EndChild();
			}
			PopStyleVar(); // ImGuiStyleVar_ChildWindowRounding, 0.f
			EndChild(); // groups

			Separator();
			Dummy({ GetContentRegionAvailWidth() - 30, 0 });
			SameLine();

			// Add/remove buttons
			BeginGroup();
			PushItemWidth(-1);
			if (Button("+"))
			{
				this->bg_props.emplace_back(BodyGroupProperties());
				this->tmp_use_relative_coords.push_back(1);
				this->tmp_cols.emplace_back(TempColArray());
			}
			if (Button("-") && bg_props.size() != 0)
			{
				this->bg_props.pop_back();
				this->tmp_use_relative_coords.pop_back();
				this->tmp_cols.pop_back();
			}
			PopItemWidth();
			EndGroup(); // Add/remove buttons

			EndPopup();
		}
	}

	void StartState::MassPopup(size_t const idx)
	{
		using namespace ImGui;

		AlignFirstTextHeightToWidgets();
		Text("Minimum");
		SameLine();
		PushItemWidth(0.5f * GetContentRegionAvailWidth());
		InputDouble("solar masses##1", &this->bg_props[idx].min_mass);
		PopItemWidth();
		AlignFirstTextHeightToWidgets();
		Text("Maximum");
		SameLine();
		PushItemWidth(0.5f * GetContentRegionAvailWidth());
		InputDouble("solar masses##2", &this->bg_props[idx].max_mass);
		PopItemWidth();
		Dummy({ 100, 0 });
		SameLine();
		if (Button("OK"))
			CloseCurrentPopup();
		SameLine();
		Dummy({ 100, 0 });
	}

	void StartState::CentralMassPopup(size_t const idx)
	{
		using namespace ImGui;

		auto static input_value = this->bg_props[idx].central_mass / this->sim->SOLAR_MASS * 1E6;
		auto static label = "1E6 solar masses";
		PushItemWidth(GetContentRegionAvailWidth() - CalcTextSize(label).x);
		InputDouble(label, &input_value);
		Dummy({ 100, 0 });
		SameLine();
		if (Button("OK"))
		{
			this->bg_props[idx].central_mass = input_value * this->sim->SOLAR_MASS * 1E6;
			CloseCurrentPopup();
		}
		SameLine();
		Dummy({ 100, 0 });
	}

	void StartState::PosVelPopup(size_t const idx)
	{
		using namespace ImGui;
		Vector2d static input_pos;
		Vector2d static input_vel;
		Checkbox("Use relative coordinates", (bool*)&this->tmp_use_relative_coords[idx]);
		SameLine();
		ShowHelpMarker("If checked, positions and velocities entered will be scaled by the universe radius %.0e",
			this->sim->RADIUS);
		PushItemWidth(GetContentRegionAvailWidth() - CalcTextSize("        ").x);
		InputDouble2("Position", &input_pos.x);
		InputDouble2("Velocity", &input_vel.x);
		PopItemWidth();
		Dummy({ 100,0 });
		SameLine();
		if (Button("OK"))
		{
			this->bg_props[idx].pos = input_pos;
			this->bg_props[idx].vel = input_vel;
			if (this->tmp_use_relative_coords[idx])
			{
				this->bg_props[idx].pos *= this->sim->RADIUS;
				this->bg_props[idx].vel *= this->sim->RADIUS;
			}
			CloseCurrentPopup();
		}
		SameLine();
		Dummy({ 100,0 });
	}

	void StartState::ColourPopup(size_t const idx)
	{
		using namespace ImGui;

		PushItemWidth(200 - CalcTextSize("Colours").x);
		auto sel_dist = (int*)(&bg_props[idx].colour);
		if (Combo("Colours", sel_dist, getColourerName,
			(void*)(this->colour_infos.data()), static_cast<int>(this->colour_infos.size())))
		{
			this->bg_props[idx].ncols = this->colour_infos[*sel_dist].cols_used;
		}
		PopItemWidth();
		if (IsItemHovered() && *sel_dist != -1)
		{
			BeginTooltip();
			PushTextWrapPos(200);
			TextWrapped(this->colour_infos[*sel_dist].tooltip);
			PopTextWrapPos();
			EndTooltip();
		}
		PushItemWidth(GetContentRegionAvailWidth());
		if (*sel_dist != -1)
		{
			for (size_t i = 0; i < this->colour_infos[*sel_dist].cols_used; i++)
			{
				char label[16];
				sprintf_s<16>(label, "Colour %zu", i);

				auto& this_col = this->tmp_cols[idx].cols[*sel_dist][i];
				if (ColorEdit3(label, this_col))
				{
					this->bg_props[idx].cols[i] = { static_cast<sf::Uint8>(this_col[0] * 255), static_cast<sf::Uint8>(this_col[1] * 255), static_cast<sf::Uint8>(this_col[2] * 255), 255 };
				}
			}
		}
		PopItemWidth();

		if (Button("OK"))
		{
			CloseCurrentPopup();
		}
	}
}