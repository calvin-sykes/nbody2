#include "Config.h"
#include "Error.h"
#include "StartState.h"
#include "RunState.h"
#include "IState.h"

#include "imgui_additional.h"
#include "imgui_sfml.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <fstream>
#include <numeric>

namespace nbody
{
	StartState::StartState(Sim * simIn)
		: m_l1_modal_open(false),
		m_l2_modal_open(false),
		m_style(ImGui::GetStyle()),
		m_bg_props(simIn->m_sim_props.bg_props),
		m_sim_props(simIn->m_sim_props)
	{
		m_sim = simIn;
		auto pos = sf::Vector2f{ m_sim->m_window.getSize() };
		m_view.setSize(pos);
		m_view.setCenter(0.5f * pos);

		// Set layout flags for the GUI windows
		m_window_flags = ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings;

		// Centre-align the GUI window titles
		m_style.WindowTitleAlign = { 0.5f, 0.5f };

		// Start at the initial menu, which prompts to load or create
		// a set of initial conditions
		m_menu_state = MenuState::INITIAL;
		m_do_run = false;
	}

	void StartState::draw(sf::Time const dt)
	{
		m_sim->m_window.resetGLStates();
		m_sim->m_window.setView(m_view);
		m_sim->m_window.clear(sf::Color::Black);
		m_sim->m_window.draw(m_sim->m_background);
		ImGui::Render();
	}

	void StartState::update(sf::Time const dt)
	{
		ImGui::SFML::Update(m_sim->m_window, dt);

		makeInitialWindow();

		if (m_do_run)
		{
			m_sim->setProperties(m_sim_props);
			m_sim->pushState(new RunState(m_sim));
			m_do_run = false;
		}

		if (m_menu_state == MenuState::CREATE_NEW)
		{
			m_l1_modal_open = true;
			makeGenerateWindow();
		}
		else if (m_menu_state == MenuState::LOAD_EXISTING)
		{
			m_l1_modal_open = true;
			makeLoadWindow();
		}

		if (!m_l1_modal_open)
		{
			m_menu_state = MenuState::INITIAL;
		}
	}

	void StartState::handleInput()
	{
		sf::Event event;

		while (m_sim->m_window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			switch (event.type)
			{
			case sf::Event::Closed:
			{
				m_sim->m_window.close();
				break;
			}
			case sf::Event::Resized:
			{
				m_view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
				m_sim->m_background.setPosition(m_sim->m_window.mapPixelToCoords(sf::Vector2i(0, 0), m_view));
				m_sim->m_background.setScale(
					float(event.size.width) / float(m_sim->m_background.getTexture()->getSize().x),
					float(event.size.height) / float(m_sim->m_background.getTexture()->getSize().y));
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape)
					m_sim->m_window.close();
			}
			default:
				break;
			}
		}
	}

	bool StartState::checkRun(std::string & result_message) const
	{
		/* things to check
		- bodies > 0 y
		- distribution != invalid y
		- radius > 0 y
		- colourer != invalid y
		- integrator != invalid y
		- algorithm != invalid y
		*/
		for (auto const& bgp : m_bg_props)
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
		if (m_sim_props.int_type == IntegratorType::INVALID)
		{
			result_message = "An integration algorithm must be selected";
			return false;
		}
		if (m_sim_props.mod_type == ModelType::INVALID)
		{
			result_message = "An evolution algorithm must be selected";
			return false;
		}
		return true;
	}

	void StartState::makeInitialWindow()
	{
		using namespace ImGui;

		SetNextWindowPosCenter();
		SetNextWindowSize(ImVec2{ 500, 300 });
		Begin("N-body Simulator", nullptr, m_window_flags);
		Dummy({ GetContentRegionAvailWidth(), 50 });
		BeginGroup();
		if (CentredButton("Load existing\ninitial conditions", { GetContentRegionAvailWidth() * 0.5f, 50 }))
		{
			m_menu_state = MenuState::LOAD_EXISTING;
		}
		SameLine();
		if (CentredButton("Generate new\ninitial conditions", { GetContentRegionAvailWidth(), 50 }))
		{
			m_menu_state = MenuState::CREATE_NEW;
		}
		EndGroup();
		Dummy({ GetContentRegionAvailWidth(), 50 });
		Dummy({ GetContentRegionAvailWidth() / 3, 50 });
		SameLine();
		if (Button("Quit", { GetContentRegionAvailWidth() * 0.5f, 50 }))
		{
			m_sim->m_window.close();
		}
		End();
	}

	void StartState::makeLoadWindow()
	{
		using namespace ImGui;

		SetNextWindowPosCenter();
		OpenPopup("Load initial conditions");
		if (BeginPopupModal("Load initial conditions", &m_l1_modal_open, m_window_flags))
		{
			char static filename[256];
			InputText("Filename", filename, 256);
			SetCursorPosX(0.5f * GetWindowContentRegionWidth());
			if (Button("OK"))
			{
				if (loadSettings(filename) == true)
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Loaded");
				}
				else
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Error##load");
				}
			}
			if (BeginPopupModal("Error##load", &m_l2_modal_open, m_window_flags))
			{
				Text("Failed to load data from file.");
				Text(m_err_string.c_str());
				SetCursorPosX(0.5f * GetWindowContentRegionWidth());
				if (Button("OK"))
				{
					CloseCurrentPopup();
				}
				EndPopup();
			}
			if (BeginPopupModal("Loaded", &m_l2_modal_open, m_window_flags))
			{
				Text("File successfully loaded.");
				SetCursorPosX(0.5f * GetWindowContentRegionWidth());
				if (Button("OK"))
				{
					m_menu_state = MenuState::CREATE_NEW;
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
		if (BeginPopupModal("Generate initial conditions", &m_l1_modal_open, m_window_flags))
		{
			// Main display of BodyGroup properties
			BeginChild("groups", { 0, 400 });
			// Show prompt if no BodyGroups have been added
			if (m_bg_props.size() == 0)
			{
				Text("Click \"+\" to add a group of bodies.");
			}
			float bg_size = 0;
			if (m_bg_props.size() > 0)
				bg_size = std::max(static_cast<float>((400 - 2 * m_style.FramePadding.y - (m_bg_props.size() - 1) * m_style.ItemSpacing.y) / m_bg_props.size()), 70.f);
			// Groups are bounded by square-cornered rectangles
			PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 0.f);
			for (size_t i = 0; i < m_bg_props.size(); i++)
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
				SliderInt("Number of bodies", &(m_bg_props[i].num), 0, Constants::MAX_N);
				PopItemWidth();
				auto n_total = std::accumulate(m_bg_props.cbegin(), m_bg_props.cend(), 0, [](auto sum, auto const& b) -> int { return sum + b.num; });
				m_sim_props.n_bodies = n_total;
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
						auto& subtractable = m_bg_props[(i + j) % (m_bg_props.size())].num;
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
				auto sel_dist = reinterpret_cast<int*>(&m_bg_props[i].dist);
				if (Combo("Distribution", sel_dist, m_getDistributorName,
					static_cast<void*>(m_dist_infos.data()), static_cast<int>(m_dist_infos.size())))
				{
					m_bg_props[i].has_central_mass = m_dist_infos[*sel_dist].has_central_mass;
				}
				PopItemWidth();
				if (IsItemHovered() && *sel_dist != -1)
				{
					BeginTooltip();
					PushTextWrapPos(200);
					TextWrapped(m_dist_infos[*sel_dist].tooltip);
					PopTextWrapPos();
					EndTooltip();
				}
				Dummy({ 0, 5 });

				ImVec2 btn_size(0.2f * GetContentRegionAvailWidth() - m_style.ItemSpacing.x, 0);

				// Mass range popup
				if (Button("Mass...", btn_size))
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Mass settings");
				}
				if (BeginPopupModal("Mass settings", &m_l2_modal_open, m_window_flags))
				{
					makeMassPopup(i);
					EndPopup();
				}

				// Central mass popup
				SameLine();
				if (Button("Central mass...", btn_size))
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Central mass setting");
				}
				if (BeginPopupModal("Central mass setting", &m_l2_modal_open, m_window_flags))
				{
					makeCentralMassPopup(i);
					EndPopup();
				}

				// Position and velocity popup
				SameLine();
				if (Button("Position/velocity...", btn_size))
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Position/velocity settings");
				}
				if (BeginPopupModal("Position/velocity settings", &m_l2_modal_open, m_window_flags))
				{
					makePosVelPopup(i);
					EndPopup();
				}

				// Radius popup
				SameLine();
				if (Button("Radius...", btn_size))
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Radius settings");
				}
				if (BeginPopupModal("Radius settings", &m_l2_modal_open, m_window_flags))
				{
					makeRadiusPopup(i);
					EndPopup();
				}

				// Colour settings popup
				SameLine();
				if (Button("Colour...", btn_size))
				{
					m_l2_modal_open = true;
					SetNextWindowPosCenter();
					OpenPopup("Colour settings");
				}
				if (BeginPopupModal("Colour settings", &m_l2_modal_open, m_window_flags))
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
			auto sel_int = reinterpret_cast<int*>(&m_sim_props.int_type);
			Combo("Integrator", sel_int, m_getIntegratorName,
				static_cast<void*>(m_integrator_infos.data()), static_cast<int>(m_integrator_infos.size()));

			// Model combobox
			auto sel_ev = reinterpret_cast<int*>(&m_sim_props.mod_type);
			Combo("Algorithm", sel_ev, m_getModelName,
				static_cast<void*>(m_model_infos.data()), static_cast<int>(m_model_infos.size()));
			if (IsItemHovered() && *sel_ev != -1)
			{
				BeginTooltip();
				PushTextWrapPos(200);
				TextWrapped(m_model_infos[*sel_ev].tooltip);
				PopTextWrapPos();
				EndTooltip();
			}
			PopItemWidth();
			EndGroup();
			auto sz = GetItemRectSize();
			SameLine();

			// Timestep
			PushItemWidth(sz.x - CalcTextSize("Timestep").x);
			static double dt_in = m_sim_props.timestep == -1. ? 1 : m_sim_props.timestep * 1e-10;
			if (InputDouble("Timestep", &dt_in) || (m_sim_props.timestep == -1.))
			{
				m_sim_props.timestep = dt_in * 1e10;
			}
			PopItemWidth();
			SameLine();
			Dummy({ 207 - sz.x + 0 * CalcTextSize("Timestep").x, 0 });
			SameLine();

			// Save button
			if (Button("Save", { 100, sz.y }))
			{
				m_l2_modal_open = true;
				OpenPopup("Save settings");
			}
			if (BeginPopupModal("Save settings", &m_l2_modal_open, m_window_flags))
			{
				makeSavePopup();
				EndPopup();
			}
			SameLine();

			// Run button
			static std::string check_result;
			if (Button("Run", { 100, sz.y }))
			{
				if (checkRun(check_result))
				{
					m_do_run = true;
				}
				else
				{
					m_l2_modal_open = true;
					OpenPopup("Error##run");
				}
			}
			if (BeginPopupModal("Error##run", &m_l2_modal_open, m_window_flags))
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
				m_bg_props.emplace_back();
			}
			if (Button("-", { GetContentRegionAvailWidth(), 0 }))
			{
				m_bg_props.pop_back();
			}
			EndGroup(); // Add/remove buttons

			EndPopup(); // Generate initial conditions
		}
	}

	void StartState::makeMassPopup(size_t const idx) const
	{
		using namespace ImGui;
		static bool share_values = false;
		AlignFirstTextHeightToWidgets();
		auto text_width_min = CalcTextSize("Minimum").x;
		auto text_width_max = CalcTextSize("Maximum").x;
		auto text_width_used = std::max(text_width_min, text_width_max);
		auto spacing = m_style.ItemSpacing.x;
		Text("Minimum");
		SameLine();
		Dummy({ text_width_used - text_width_min - spacing, 0 });
		SameLine();
		PushItemWidth(80.f);
		InputDouble("solar masses##1", &m_bg_props[idx].min_mass);
		PopItemWidth();
		AlignFirstTextHeightToWidgets();
		Text("Maximum");
		SameLine();
		Dummy({ text_width_used - text_width_max - spacing, 0 });
		SameLine();
		PushItemWidth(80.f);
		InputDouble("solar masses##2", &m_bg_props[idx].max_mass);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : m_bg_props)
				{
					bgp.min_mass = m_bg_props[idx].min_mass;
					bgp.max_mass = m_bg_props[idx].max_mass;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeCentralMassPopup(size_t const idx) const
	{
		using namespace ImGui;
		static bool share_values = false;
		auto static label = "1E6 solar masses";
		PushItemWidth(80.0f);
		InputDouble(label, &m_bg_props[idx].central_mass);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : m_bg_props)
				{
					bgp.central_mass = m_bg_props[idx].central_mass;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makePosVelPopup(size_t const idx) const
	{
		using namespace ImGui;
		static bool share_values = false;
		Checkbox("Use relative coordinates", &m_bg_props[idx].use_relative_coords);
		SameLine();
		ShowHelpMarker("If checked, the entered position will be interpreted as a fraction of the universe radius %.0em",
			Constants::RADIUS);
		PushItemWidth(2 * (80.f + m_style.ItemInnerSpacing.x));
		InputDouble2("Position", &m_bg_props[idx].pos.x);
		InputDouble2("Velocity", &m_bg_props[idx].vel.x);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : m_bg_props)
				{
					bgp.pos = m_bg_props[idx].pos;
					bgp.vel = m_bg_props[idx].vel;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeRadiusPopup(size_t const idx) const
	{
		using namespace ImGui;
		static bool share_values = false;
		Checkbox("Use relative coordinates", &m_bg_props[idx].use_relative_coords);
		SameLine();
		ShowHelpMarker("If checked, the radius entered will be interpreted as a fraction of the universe radius %.0em",
			Constants::RADIUS);
		PushItemWidth(80.0f);
		InputDouble("Radius", &m_bg_props[idx].radius);
		PopItemWidth();
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : m_bg_props)
				{
					bgp.radius = m_bg_props[idx].radius;
				}
			}
			CloseCurrentPopup();
		}
	}

	void StartState::makeColourPopup(size_t const idx)
	{
		using namespace ImGui;
		static bool share_values;
		auto sel_col = reinterpret_cast<int*>(&m_bg_props[idx].colour);
		if (Combo("Colour method", sel_col, m_getColourerName,
			static_cast<void*>(m_colour_infos.data()), static_cast<int>(m_colour_infos.size())))
		{
			SetWindowSize({ 0, 0 });
		}
		if (IsItemHovered() && *sel_col != -1)
		{
			BeginTooltip();
			PushTextWrapPos(200);
			TextWrapped(m_colour_infos[*sel_col].tooltip);
			PopTextWrapPos();
			EndTooltip();
		}
		if (*sel_col != -1)
		{
			for (size_t i = 0; i < m_colour_infos[*sel_col].cols_used; i++)
			{
				char label[16];
#ifdef SAFE_STRFN
				sprintf_s<16>(label, "Colour %zu", i + 1);
#else
				sprintf(label, "Colour %zu", i + 1);
#endif
				auto this_col = &m_bg_props[idx].cols[i];
				ColorEdit3_sf(label, this_col);
			}
		}
		Checkbox("Same for all groups", &share_values);
		SetCursorPosX(0.5f * GetWindowContentRegionWidth());
		if (Button("OK"))
		{
			if (share_values)
			{
				for (auto & bgp : m_bg_props)
				{
					bgp.colour = m_bg_props[idx].colour;
					for(auto i = 0; i < MAX_COLS_PER_COLOURER; i++)
					{
						bgp.cols[i] = m_bg_props[idx].cols[i];
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
		std::ofstream file;
		auto writeString = [&file](char const data[], size_t len) -> void
		{
			file.write(data, len);
			file.write(fileio::SEP, sizeof(fileio::SEP));
		};

		auto writeValue = [&file](auto data) -> void
		{
			file.write(reinterpret_cast<char*>(&data), sizeof(decltype(data)));
			file.write(fileio::SEP, sizeof(fileio::SEP));
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
			writeString(fileio::FILE_HEADER, fileio::SIZE_FH);
			writeString(fileio::VERSION, fileio::SIZE_VER);
			writeString(fileio::GLOBAL_HEADER, fileio::SIZE_GH);
			writeValue(m_sim_props.timestep);
			writeValue(m_sim_props.n_bodies);
			writeValue(m_sim_props.int_type);
			writeValue(m_sim_props.mod_type);
			writeValue(m_bg_props.size());
			std::for_each(m_bg_props.begin(), m_bg_props.end(), [&](BodyGroupProperties const& bgp) {
				writeString(fileio::ITEM_HEADER, fileio::SIZE_IH);
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
		//using namespace fileio;
		std::ifstream file;
		auto readString = [&file](char const data[], size_t len) -> bool
		{
			auto buf = new char[len + 1];
			file.read(buf, len);
			auto str_read = !strcmp(data, buf);
			delete[] buf;
			char buf2[sizeof(fileio::SEP) + 1];
			file.read(buf2, sizeof(fileio::SEP));
			return str_read & !strcmp(buf2, fileio::SEP);
		};
		auto readValue = [&file](auto&& dest) -> bool
		{
			auto len = sizeof(dest);
			auto buf = new char[len];
			file.read(buf, len);
			dest = reinterpret_cast<decltype(dest)>(*buf);
			delete[] buf;
			char buf2[sizeof(fileio::SEP) + 1];
			file.read(buf2, sizeof(fileio::SEP));
			return !strcmp(buf2, fileio::SEP);

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

			auto good = readString(fileio::FILE_HEADER, fileio::SIZE_FH);
			good &= readString(fileio::VERSION, fileio::SIZE_VER);
			good &= readString(fileio::GLOBAL_HEADER, fileio::SIZE_GH);
			if (!good)
				throw MAKE_ERROR(std::string("could not read header of file ") + fn_str);
			good &= readValue(m_sim_props.timestep);
			good &= readValue(m_sim_props.n_bodies);
			good &= readValue(m_sim_props.int_type);
			good &= readValue(m_sim_props.mod_type);
			if (!good)
				throw MAKE_ERROR(std::string("could not read global properties in file ") + fn_str);
			size_t n_groups;
			readValue(n_groups);
			m_bg_props.assign(n_groups, BodyGroupProperties());
			std::for_each(m_bg_props.begin(), m_bg_props.end(), [&](BodyGroupProperties& bgp) {
				good &= readString(fileio::ITEM_HEADER, fileio::SIZE_IH);
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
				for (auto& colour : bgp.cols)
				{
					good &= readValue(colour);
				}
				if (!good)
					throw MAKE_ERROR(std::string("could not read BodyGroup properties in file ") + fn_str);
			});
			file.close();
			return good;
		}
		catch (Error e)
		{
			file.close();
			m_err_string = e.what();
			return false;
		}
	}
}