#include "Config.h"
#include "Error.h"
#include "StartState.h"
#include "RunState.h"
#include "SimState.h"

#include "imgui_sfml.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <numeric>
#include <stdarg.h>

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
		strncpy(label_cpy, label, len + 1);
#ifdef SAFE_STRFN
		auto tok = strtok_s(label_cpy, "\n", &next_tok);
		while (tok != nullptr)
		{
			lines[n_lines] = new char[strlen(tok) + 1];
			strncpy(lines[n_lines++], tok, strlen(tok) + 1);
			tok = strtok_s(nullptr, "\n", &next_tok);
		}
#else
		auto tok = strtok(label_cpy, "\n");
		while (tok != nullptr)
		{
			lines[n_lines] = new char[strlen(tok) + 1];
			strncpy(lines[n_lines++], tok, strlen(tok) + 1);
			tok = strtok(nullptr, "\n");
		}
#endif

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
#ifdef SAFE_STRFN
			strncpy_s(formatted + pos, formatted_len - pos, lines[i], line_lens[i]);
#else
			strncpy(formatted + pos, lines[i], line_lens[i]);
#endif		
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
	StartState::StartState(Sim * simIn) :
		style(ImGui::GetStyle()), sim_props(simIn->m_sim_props), bg_props(simIn->m_sim_props.bg_props)
	{
		this->sim = simIn;
		sf::Vector2f pos = sf::Vector2f(this->sim->m_window.getSize());
		this->view.setSize(pos);
		this->view.setCenter(0.5f * pos);

		// Set layout flags for the GUI windows
		this->window_flags = ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings;

		// Centre-align the GUI window titles
		this->style.WindowTitleAlign = { 0.5f, 0.5f };

		// Start at the initial menu, which prompts to load or create
		// a set of initial conditions
		this->menu_state = MenuState::INITIAL;
		this->do_run = false;
	}

	void StartState::draw(sf::Time const dt)
	{
		this->sim->m_window.resetGLStates();
		this->sim->m_window.setView(this->view);
		this->sim->m_window.clear(sf::Color::Black);
		this->sim->m_window.draw(this->sim->m_background);
		ImGui::Render();
	}

	void StartState::update(sf::Time const dt)
	{
		ImGui::SFML::Update(this->sim->m_window, dt);

		makeInitialWindow();

		if (this->do_run)
		{
			this->sim->setProperties(this->sim_props);
			this->run();
			this->do_run = false;
		}

		if (this->menu_state == MenuState::CREATE_NEW)
		{
			l1_modal_is_open = true;
			makeGenerateWindow();
		}
		else if (this->menu_state == MenuState::LOAD_EXISTING)
		{
			l1_modal_is_open = true;
			makeLoadWindow();
		}

		if (!l1_modal_is_open)
		{
			menu_state = MenuState::INITIAL;
		}
	}

	void StartState::handleInput()
	{
		sf::Event event;

		while (this->sim->m_window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			switch (event.type)
			{
			case sf::Event::Closed:
			{
				this->sim->m_window.close();
				break;
			}
			case sf::Event::Resized:
			{
				this->view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
				this->sim->m_background.setPosition(this->sim->m_window.mapPixelToCoords(sf::Vector2i(0, 0), this->view));
				this->sim->m_background.setScale(
					float(event.size.width) / float(this->sim->m_background.getTexture()->getSize().x),
					float(event.size.height) / float(this->sim->m_background.getTexture()->getSize().y));
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape)
					this->sim->m_window.close();
			}
			default:
				break;
			}
		}
	}

	bool StartState::checkRun(std::string & result_message)
	{
		/* things to check
		- bodies > 0 y
		- distribution != invalid y
		- radius > 0 y
		- colourer != invalid y
		- integrator != invalid y
		- algorithm != invalid y
		*/
		for (auto const& bgp : this->bg_props)
		{
			if (bgp.num == 0)
			{
				result_message = "Every group must contain at least one body";
				return false;
			}
			if (bgp.radius == 0.0)
			{
				result_message = "Every group must have a non-zero radius";
				return false;
			}
			if (bgp.dist == DistributorType::INVALID)
			{
				result_message = "A distribution must be selected for each group of bodies";
				return false;
			}
			if (bgp.colour == ColourerType::INVALID)
			{
				result_message = "A colour scheme must be selected for each group of bodies";
				return false;
			}
		}
		if (this->sim_props.int_type == IntegratorType::INVALID)
		{
			result_message = "An integration algorithm must be selected";
			return false;
		}
		/*if (this->sim_props.ev_type == EvolverType::INVALID)
		{
			result_message = "An evolution algorithm must be selected";
			return false;
		}*/
		if (this->sim_props.mod_type == ModelType::INVALID)
		{
			result_message = "An evolution algorithm must be selected";
			return false;
		}
		return true;
	}

	void StartState::run()
	{
		this->sim->pushState(new RunState(this->sim));
	}

	void StartState::makeInitialWindow()
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
			this->sim->m_window.close();
		}
		End();
	}

	void StartState::makeLoadWindow()
	{
		using namespace ImGui;

		SetNextWindowPosCenter();
		OpenPopup("Load initial conditions");
		if (BeginPopupModal("Load initial conditions", &l1_modal_is_open, this->window_flags))
		{
			char static filename[256];
			InputText("Filename", filename, 256);
			SetCursorPosX(0.5f * GetWindowContentRegionWidth());
			if (Button("OK"))
			{
				if (loadSettings(filename) == true)
				{
					this->l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Loaded");
				}
				else
				{
					this->l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Error##load");
				}
			}
			if (BeginPopupModal("Error##load", &l2_modal_is_open, this->window_flags))
			{
				Text("Failed to load data from file.");
				Text(this->err_string.c_str());
				SetCursorPosX(0.5f * GetWindowContentRegionWidth());
				if (Button("OK"))
				{
					CloseCurrentPopup();
				}
				EndPopup();
			}
			if (BeginPopupModal("Loaded", &l2_modal_is_open, this->window_flags))
			{
				Text("File successfully loaded.");
				SetCursorPosX(0.5f * GetWindowContentRegionWidth());
				if (Button("OK"))
				{
					this->menu_state = MenuState::CREATE_NEW;
					CloseCurrentPopup();
				}
				EndPopup();
			}
			EndPopup();
		}
	}

	void StartState::makeGenerateWindow()
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
			float bg_size;
			if (this->bg_props.size() > 0)
				bg_size = max(static_cast<float>((400 - 2 * style.FramePadding.y - (this->bg_props.size() - 1) * style.ItemSpacing.y) / this->bg_props.size()), 70.f);
			// Groups are bounded by square-cornered rectangles
			PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 0.f);
			for (size_t i = 0; i < this->bg_props.size(); i++)
			{
				// Create unique ID for group
				char entry_id[16] = {};
#ifdef SAFE_STRFN
				sprintf_s<16>(entry_id, "group##%zu", i);
#else
				sprintf(entry_id, "group##%zu", i);
#endif
				// Single set of BodyGroup properties //
				BeginChild(entry_id, { 0, bg_size }, true);
				// Number label
				Text("Group %zu", i + 1);
				// Number of bodies slider
				SameLine();
				PushItemWidth(0.5f * GetContentRegionAvailWidth());
				SliderInt("Number of bodies", &(this->bg_props[i].num), 0, Constants::MAX_N);
				PopItemWidth();
				auto n_total = std::accumulate(bg_props.cbegin(), bg_props.cend(), 0, [](auto sum, auto const& b) -> int { return sum + b.num; });
				this->sim_props.n_bodies = n_total;
				// too many bodies, need to redistribute them
				if (n_total > Constants::MAX_N)
				{
					// if too many, steal from previous, except if first, then steal from last
					// if more need to be stolen than are available, move to next
					auto n_excess = n_total - static_cast<int>(Constants::MAX_N);
					size_t j = 1;
					while (n_excess > 0)
					{
						// how many are available to steal?
						// take as reference so we can modify it
						auto& subtractable = this->bg_props[(i + j) % (this->bg_props.size())].num;
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
				ImVec2 btn_size(0.2f * GetContentRegionAvailWidth() - style.ItemSpacing.x, 0);
				// Mass range popup
				if (Button("Mass...", btn_size))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Mass settings");
				}
				if (BeginPopupModal("Mass settings", &l2_modal_is_open, window_flags))
				{
					makeMassPopup(i);
					EndPopup();
				}
				// Central mass popup
				SameLine();
				if (Button("Central mass...", btn_size))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Central mass setting");
				}
				if (BeginPopupModal("Central mass setting", &l2_modal_is_open, window_flags))
				{
					makeCentralMassPopup(i);
					EndPopup();
				}
				// Position and velocity popup
				SameLine();
				if (Button("Position/velocity...", btn_size))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Position/velocity settings");
				}
				if (BeginPopupModal("Position/velocity settings", &l2_modal_is_open, window_flags))
				{
					makePosVelPopup(i);
					EndPopup();
				}
				// Radius popup
				SameLine();
				if (Button("Radius...", btn_size))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Radius settings");
				}
				if (BeginPopupModal("Radius settings", &l2_modal_is_open, window_flags))
				{
					makeRadiusPopup(i);
					EndPopup();
				}
				// Colour settings popup
				SameLine();
				if (Button("Colour...", btn_size))
				{
					l2_modal_is_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Colour settings");
				}
				if (BeginPopupModal("Colour settings", &l2_modal_is_open, window_flags))
				{
					makeColourPopup(i);
					EndPopup();
				}

				EndChild();
			}
			PopStyleVar(); // ImGuiStyleVar_ChildWindowRounding, 0.f
			EndChild(); // BodyGroups
			Separator();
			BeginGroup();
			// Integrator combobox
			PushItemWidth(150);
			auto sel_int = (int*)(&this->sim_props.int_type);
			Combo("Integrator", sel_int, getIntegratorName,
				(void*)(this->integrator_infos.data()), static_cast<int>(this->integrator_infos.size()));
			// Evolution method combobox
			/*auto sel_ev = (int*)(&this->sim_props.ev_type);
			Combo("Algorithm", sel_ev, getEvolveName,
				(void*)(this->evolve_infos.data()), static_cast<int>(this->evolve_infos.size()));
			if (IsItemHovered() && *sel_ev != -1)
			{
				BeginTooltip();
				PushTextWrapPos(200);
				TextWrapped(this->evolve_infos[*sel_ev].tooltip);
				PopTextWrapPos();
				EndTooltip();
			}*/
			auto sel_ev = (int*)(&this->sim_props.mod_type);
			Combo("Algorithm", sel_ev, getModelName,
				(void*)(this->model_infos.data()), static_cast<int>(this->model_infos.size()));
			if (IsItemHovered() && *sel_ev != -1)
			{
				BeginTooltip();
				PushTextWrapPos(200);
				TextWrapped(this->model_infos[*sel_ev].tooltip);
				PopTextWrapPos();
				EndTooltip();
			}
			PopItemWidth();
			EndGroup();
			auto sz = GetItemRectSize();
			SameLine();
			// Timestep
			PushItemWidth(sz.x - CalcTextSize("Timestep").x);
			static double dt_in = this->sim_props.timestep == -1. ? 1 : this->sim_props.timestep * 1e-10;
			if (InputDouble("Timestep", &dt_in) || (this->sim_props.timestep == -1.))
			{
				this->sim_props.timestep = dt_in * 1e10;
			}
			PopItemWidth();
			SameLine();
			Dummy({ 207 - sz.x + 0 * CalcTextSize("Timestep").x, 0 });
			SameLine();
			// Save button
			if (Button("Save", { 100, sz.y }))
			{
				l2_modal_is_open = true;
				OpenPopup("Save settings");
			}
			if (BeginPopupModal("Save settings", &l2_modal_is_open, window_flags))
			{
				makeSavePopup();
				EndPopup();
			}
			SameLine();
			// Run button
			static std::string check_result;
			if (Button("Run", { 100, sz.y }))
			{
				if (this->checkRun(check_result))
				{
					this->do_run = true;
				}
				else
				{
					l2_modal_is_open = true;
					OpenPopup("Error##run");
				}
			}
			if (BeginPopupModal("Error##run", &l2_modal_is_open, this->window_flags))
			{
				Text(check_result.c_str());
				SetCursorPosX(0.5f * GetContentRegionAvailWidth());
				if (Button("OK"))
				{
					CloseCurrentPopup();
				}
				EndPopup();
			}
			SameLine();
			// Add/remove buttons
			SameLine();
			BeginGroup();
			if (Button("+", { GetContentRegionAvailWidth(), 0 }))
			{
				this->bg_props.emplace_back();
				this->tmp_cols.emplace_back();
			}
			if (Button("-", { GetContentRegionAvailWidth(), 0 }))
			{
				this->bg_props.pop_back();
				this->tmp_cols.pop_back();
			}
			EndGroup(); // Add/remove buttons

			EndPopup(); // Generate initial conditions
		}
	}

	void StartState::makeMassPopup(size_t const idx)
	{
		using namespace ImGui;
		static bool share_values = false;
		AlignFirstTextHeightToWidgets();
		auto text_width_min = CalcTextSize("Minimum").x;
		auto text_width_max = CalcTextSize("Maximum").x;
		auto text_width_used = max(text_width_min, text_width_max);
		auto spacing = style.ItemSpacing.x;
		Text("Minimum");
		SameLine();
		Dummy({ text_width_used - text_width_min - spacing, 0 });
		SameLine();
		PushItemWidth(80.f);
		InputDouble("solar masses##1", &this->bg_props[idx].min_mass);
		PopItemWidth();
		AlignFirstTextHeightToWidgets();
		Text("Maximum");
		SameLine();
		Dummy({ text_width_used - text_width_max - spacing, 0 });
		SameLine();
		PushItemWidth(80.f);
		InputDouble("solar masses##2", &this->bg_props[idx].max_mass);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : this->bg_props)
				{
					bgp.min_mass = this->bg_props[idx].min_mass;
					bgp.max_mass = this->bg_props[idx].max_mass;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeCentralMassPopup(size_t const idx)
	{
		using namespace ImGui;
		static bool share_values = false;
		auto static label = "1E6 solar masses";
		PushItemWidth(80.0f);
		InputDouble(label, &this->bg_props[idx].central_mass);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : this->bg_props)
				{
					bgp.central_mass = this->bg_props[idx].central_mass;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makePosVelPopup(size_t const idx)
	{
		using namespace ImGui;
		static bool share_values = false;
		Checkbox("Use relative coordinates", &this->bg_props[idx].use_relative_coords);
		SameLine();
		ShowHelpMarker("If checked, the entered position will be interpreted as a fraction of the universe radius %.0em",
			Constants::RADIUS);
		PushItemWidth(2 * (80.f + this->style.ItemInnerSpacing.x));
		InputDouble2("Position", &this->bg_props[idx].pos.x);
		InputDouble2("Velocity", &this->bg_props[idx].vel.x);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : this->bg_props)
				{
					bgp.pos = this->bg_props[idx].pos;
					bgp.vel = this->bg_props[idx].vel;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeRadiusPopup(size_t const idx)
	{
		using namespace ImGui;
		static bool share_values = false;
		Checkbox("Use relative coordinates", &this->bg_props[idx].use_relative_coords);
		SameLine();
		ShowHelpMarker("If checked, the radius entered will be interpreted as a fraction of the universe radius %.0em",
			Constants::RADIUS);
		PushItemWidth(80.0f);
		InputDouble("Radius", &this->bg_props[idx].radius);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : this->bg_props)
				{
					bgp.radius = this->bg_props[idx].radius;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeColourPopup(size_t const idx)
	{
		using namespace ImGui;
		static bool share_values;
		auto sel_col = (int*)(&bg_props[idx].colour);
		if (Combo("Colour method", sel_col, getColourerName,
			(void*)(this->colour_infos.data()), static_cast<int>(this->colour_infos.size())))
		{
			SetWindowSize({ 0, 0 });
		}
		if (IsItemHovered() && *sel_col != -1)
		{
			BeginTooltip();
			PushTextWrapPos(200);
			TextWrapped(this->colour_infos[*sel_col].tooltip);
			PopTextWrapPos();
			EndTooltip();
		}
		if (*sel_col != -1)
		{
			for (size_t i = 0; i < this->colour_infos[*sel_col].cols_used; i++)
			{
				char label[16];
#ifdef SAFE_STRFN
				sprintf_s<16>(label, "Colour %zu", i + 1);
#else
				sprintf(label, "Colour %zu", i + 1);
#endif
				auto& this_col = this->tmp_cols[idx].cols[*sel_col][i];
				if (ColorEdit3(label, this_col))
				{
					this->bg_props[idx].cols[i] =
					{ static_cast<sf::Uint8>(this_col[0] * 255), static_cast<sf::Uint8>(this_col[1] * 255), static_cast<sf::Uint8>(this_col[2] * 255), 255 };
				}
			}
		}
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				size_t bgp_ctr = 0;
				for (auto & bgp : this->bg_props)
				{
					bgp.colour = this->bg_props[idx].colour;
					for (size_t i = 0; i < this->colour_infos[*sel_col].cols_used; i++)
					{
						bgp.cols[i] = this->bg_props[idx].cols[i];
						auto& src_col = this->tmp_cols[idx].cols[*sel_col][i];
						auto& target_col = this->tmp_cols[bgp_ctr++].cols[*sel_col][i];
						target_col[0] = src_col[0];
						target_col[1] = src_col[1];
						target_col[2] = src_col[2];
					}
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeSavePopup()
	{
		using namespace ImGui;

		char static filename[256] = {};
		InputText("Filename", filename, 256);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			saveSettings(filename);
			CloseCurrentPopup();
		}
	}

	void StartState::saveSettings(char const* filename)
	{
		using namespace fileio;
		std::ofstream file;
		auto writeString = [&file](char const data[], size_t len) -> void
		{
			file.write(data, len);
			file.write(SEP, sizeof(SEP));
		};

		auto writeValue = [&file](auto data) -> void
		{
			file.write(reinterpret_cast<char*>(&data), sizeof(decltype(data)));
			file.write(SEP, sizeof(SEP));
		};

		// if a file extension was supplied, trim it and append '.dat. instead
		std::string fn_str(filename);
		auto pos = fn_str.find_last_of('.');
		if (pos != std::string::npos)
		{
			fn_str.erase(pos);
		}
		fn_str.append(".dat");

		file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

		try
		{
			file.open(fn_str, std::ios::binary);
			writeString(FILE_HEADER, SIZE_FH);
			writeString(VERSION, SIZE_VER);
			writeString(GLOBAL_HEADER, SIZE_GH);
			writeValue(this->sim_props.timestep);
			writeValue(this->sim_props.n_bodies);
			writeValue(this->sim_props.int_type);
			//writeValue(this->sim_props.ev_type);
			writeValue(this->sim_props.mod_type);
			writeValue(this->bg_props.size());
			std::for_each(bg_props.begin(), bg_props.end(), [&](auto& bgp) {
				writeString(ITEM_HEADER, SIZE_IH);
				writeValue(bgp.dist);
				writeValue(bgp.num);
				writeValue(bgp.pos);
				writeValue(bgp.vel);
				writeValue(bgp.radius);
				writeValue(bgp.use_relative_coords);
				writeValue(bgp.min_mass);
				writeValue(bgp.max_mass);
				writeValue(bgp.has_central_mass);
				writeValue(bgp.central_mass);
				writeValue(bgp.colour);
				for (auto c : bgp.cols)
				{
					writeValue(c);
				}
			});
			file.close();
		}
		catch (std::ofstream::failure const& fail)
		{
			throw MAKE_ERROR(fail.what());
		}

	}

	bool StartState::loadSettings(char const * filename)
	{
		using namespace fileio;
		std::ifstream file;
		auto readString = [&file](char const data[], size_t len) -> bool
		{
			auto buf = new char[len + 1];
			file.read(buf, len);
			auto str_read = !strcmp(data, buf);
			delete[] buf;
			char buf2[sizeof(SEP) + 1];
			file.read(buf2, sizeof(SEP));
			return str_read & !strcmp(buf2, SEP);
		};
		auto readValue = [&file](auto&& dest) -> bool
		{
			auto len = sizeof(dest);
			auto buf = new char[len];
			file.read(buf, len);
			dest = reinterpret_cast<decltype(dest)>(*buf);
			delete[] buf;
			char buf2[sizeof(SEP) + 1];
			file.read(buf2, sizeof(SEP));
			return !strcmp(buf2, SEP);

		};

		std::string fn_str(filename);
		// if a file extension was supplied, trim it and append '.dat. instead
		auto pos = fn_str.find_last_of('.');
		if (pos != std::string::npos)
		{
			fn_str.erase(pos);
		}
		fn_str.append(".dat");

		try
		{
			file.open(fn_str, std::ios::binary);
			if (!file.is_open())
				throw MAKE_ERROR(std::string("file " + fn_str + " does not exist"));

			auto good = readString(FILE_HEADER, SIZE_FH);
			good &= readString(VERSION, SIZE_VER);
			good &= readString(GLOBAL_HEADER, SIZE_GH);
			if (!good)
				throw MAKE_ERROR(std::string("could not read header of file ") + fn_str);
			good &= readValue(this->sim_props.timestep);
			good &= readValue(this->sim_props.n_bodies);
			good &= readValue(this->sim_props.int_type);
			//good &= readValue(this->sim_props.ev_type);
			good &= readValue(this->sim_props.mod_type);
			if (!good)
				throw MAKE_ERROR(std::string("could not read global properties in file ") + fn_str);
			size_t n_groups;
			readValue(n_groups);
			this->bg_props.assign(n_groups, BodyGroupProperties());
			this->tmp_cols.assign(n_groups, TempColArray());
			size_t bgp_number = 0;
			auto foo = std::for_each(bg_props.begin(), bg_props.end(), [&](auto& bgp) {
				good &= readString(ITEM_HEADER, SIZE_IH);
				good &= readValue(bgp.dist);
				good &= readValue(bgp.num);
				good &= readValue(bgp.pos);
				good &= readValue(bgp.vel);
				good &= readValue(bgp.radius);
				good &= readValue(bgp.use_relative_coords);
				good &= readValue(bgp.min_mass);
				good &= readValue(bgp.max_mass);
				good &= readValue(bgp.has_central_mass);
				good &= readValue(bgp.central_mass);
				good &= readValue(bgp.colour);
				size_t colour_number = 0;
				for (auto colour : bgp.cols)
				{
					good &= readValue(colour);
					// buffer into array used by GUI
					tmp_cols[bgp_number].cols[static_cast<size_t>(bgp.colour)][colour_number][0] = static_cast<float>(colour.r) / 255;
					tmp_cols[bgp_number].cols[static_cast<size_t>(bgp.colour)][colour_number][1] = static_cast<float>(colour.g) / 255;
					tmp_cols[bgp_number].cols[static_cast<size_t>(bgp.colour)][colour_number][2] = static_cast<float>(colour.b) / 255;
					colour_number++;
				}
				bgp_number++;
				if (!good)
					throw MAKE_ERROR(std::string("could not read BodyGroup properties in file ") + fn_str);
			});
			file.close();
			return good;
		}
		catch (Error e)
		{
			file.close();
			this->err_string = e.what();
			return false;
		}
	}
}