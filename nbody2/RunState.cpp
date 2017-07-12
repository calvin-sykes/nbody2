#include "BHTreeNode.h"
#include "Config.h"
#include "Display.h"
#include "IState.h"
#include "ModelBarnesHut.h"
#include "RunState.h"
#include "Sim.h"
#include "Timings.h"

#include "specrend.h"

#include "imgui.h"
#include "imgui_sfml.h"

#include <SFML/Graphics.hpp>

namespace nbody
{
	//void drawEllipse(double const a, double const b, double const angle);
	//double eccentricity(double const r);

	std::map<Timings, std::chrono::time_point<Clock>> timings;

	RunState::RunState(Sim * simIn) :
		m_highlighted(nullptr),
		m_energy(0.0)
	{
		m_sim = simIn;
		auto pos = sf::Vector2f(m_sim->m_window.getSize());
		m_main_view.setSize(pos);
		m_gui_view.setSize(pos);
		m_main_view.setCenter(0.5f * pos);
		m_gui_view.setCenter(0.5f * pos);

		m_flags.tree_exists = m_sim->m_mod_ptr->hasTree();

		m_sim->m_mod_ptr->updateColours(m_sim->m_int_ptr->getStateVector());

		timings[Timings::RUN_START] = Clock::now();
	}

	void RunState::update(sf::Time const dt)
	{
		if (m_flags.running)
		{
			m_sim->m_int_ptr->singleStep();
			m_sim->m_mod_ptr->updateColours(m_sim->m_int_ptr->getStateVector());
		}

		timings[Timings::DRAW_BODIES_START] = Clock::now();
		if (m_flags.show_bodies)
		{
			m_body_mgr.update(
				m_sim->m_int_ptr->getStateVector(),
				m_sim->m_mod_ptr->getAuxState(),
				m_sim->m_mod_ptr->getColourState(),
				m_sim->m_mod_ptr->getNumBodies());
		}
		timings[Timings::DRAW_BODIES_END] = Clock::now();

		timings[Timings::DRAW_GRID_START] = Clock::now();
		if (m_flags.tree_exists && m_flags.show_grid)
		{
			auto mouse_pos = sf::Mouse::getPosition(m_sim->m_window);
			auto mouse_world = Vector2d{ Display::screenToWorldX(static_cast<float>(mouse_pos.x)), Display::screenToWorldY(static_cast<float>(mouse_pos.y)) };
			m_highlighted = m_sim->m_mod_ptr->getTreeRoot()->getHovered(mouse_world);

			// if only drawing nodes used for force calculation
			// walk highlight pointer up tree until it points to a node not recursed into
			if (!m_flags.grid_mode_complete && m_highlighted)
			{
				while (!m_highlighted->getParent()->wasSubdivided())
					m_highlighted = m_highlighted->getParent();
			}

			auto mode = m_flags.grid_mode_complete ? GridDrawMode::COMPLETE : GridDrawMode::APPROX;
			m_quad_mgr.update(m_sim->m_mod_ptr->getTreeRoot(), mode, m_highlighted);
		}
		else
			m_highlighted = nullptr;
		timings[Timings::DRAW_GRID_END] = Clock::now();

		timings[Timings::DRAW_TRAILS_START] = Clock::now();
		if (m_flags.show_trails)
		{
			m_trail_mgr.update(
				m_sim->m_int_ptr->getStateVector(),
				m_sim->m_mod_ptr->getNumBodies());
		}
		timings[Timings::DRAW_TRAILS_END] = Clock::now();

		using namespace ImGui;

		SFML::Update(m_sim->m_window, dt);

		constexpr auto h_sz = 400.f;

		SetNextWindowSize({ h_sz, 0.0f }, ImGuiSetCond_FirstUseEver);
		SetNextWindowPos({ 10.f, 10.f }, ImGuiSetCond_FirstUseEver);
		SetNextWindowSizeConstraints({ h_sz, 0.f }, { h_sz, FLT_MAX });
		Begin("Diagnostics", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
		if (CollapsingHeader("Simulation statistics"))
		{
			using namespace std::chrono;

			auto constexpr SECS_IN_YEAR = 86400 * 365;

			Text("Number of steps: %zu", m_sim->m_int_ptr->getNumSteps());

			auto m_time_yrs = m_sim->m_int_ptr->getTime() / SECS_IN_YEAR;
			Text("Simulation time: %.3g yrs", m_time_yrs);

			auto elapsed = Clock::now() - timings[Timings::RUN_START];
			auto minutes = duration_cast<std::chrono::minutes>(elapsed);
			auto seconds = duration_cast<std::chrono::seconds>(elapsed);
			Text("Elapsed run time: %d:%.2zu", minutes.count(), seconds.count() % 60);
			//AlignFirstTextHeightToWidgets();
			Text("System energy: %.3g J", m_energy);
			SameLine();
			if (SmallButton("Recalculate"))
			{
				m_energy = m_sim->m_mod_ptr->getTotalEnergy(m_sim->m_int_ptr->getStateVector());
			}
			Spacing();
		}

		if (CollapsingHeader("Timings"))
		{
			using namespace std::chrono;

			auto fps = 1000.f / dt.asMilliseconds();
			auto t_tree = Dble_ms{ timings[Timings::TREE_BUILD_END] - timings[Timings::TREE_BUILD_START] };
			auto t_eval = Dble_ms{ timings[Timings::FORCE_CALC_END] - timings[Timings::FORCE_CALC_START] };
			auto t_body = Dble_ms{ timings[Timings::DRAW_BODIES_END] - timings[Timings::DRAW_BODIES_START] };
			auto t_grid = Dble_ms{ timings[Timings::DRAW_GRID_END] - timings[Timings::DRAW_GRID_START] };
			auto t_trail = Dble_ms{ timings[Timings::DRAW_TRAILS_END] - timings[Timings::DRAW_TRAILS_START] };
			auto t_render = Dble_ms{ timings[Timings::RENDER_END] - timings[Timings::RENDER_START] };
			auto t_energy = Dble_ms{ timings[Timings::ENERGY_CALC_END] - timings[Timings::ENERGY_CALC_START] };

			Text("FPS: %f", fps);
			Text("Tree construction: %f ms", t_tree.count());
			Text("Force evaluation: %f ms", t_eval.count());
			Text("Draw bodies: %f ms", t_body.count());
			Text("Draw grid: %f ms", t_grid.count());
			Text("Draw trails: %f ms", t_trail.count());
			Text("Render: %f ms", t_render.count());
			Text("Last total energy calculation: %f ms", t_energy.count());
			Spacing();
		}

		if (m_flags.tree_exists)
		{
			if (CollapsingHeader("Tree statistics"))
			{

				auto mod_bh_tree = dynamic_cast<ModelBarnesHut *>(m_sim->m_mod_ptr.get());
				auto stats = mod_bh_tree->getTreeRoot()->getStats();
				auto num_bodies = m_sim->m_mod_ptr->getNumBodies();

				Text("Force calculations for particle 0 = %zu", stats.m_num_calc);
				if (stats.m_num_calc)
					Text("Speed-up vs. brute-force case = %f", static_cast<double>(num_bodies * (num_bodies - 1)) / (2 * stats.m_num_calc * num_bodies));
				Text("Total nodes in tree = %zu", stats.m_node_ct);
				Text("Level of deepest node = %zu", stats.m_max_level);
				Text("Particles in tree = %zu", stats.m_body_ct);
				Text("Renegade particles = %zu", num_bodies - stats.m_body_ct);
				Spacing();
			}
		}

		if (CollapsingHeader("Highlighted tree node"))
		{
			if (m_highlighted)
			{
				auto const& quad = m_highlighted->getQuad();
				auto const& centre = quad.getPos();
				auto const len = quad.getLength();
				auto const& centre_mass = m_highlighted->getCentreMass();

				Text("Node: BHTreeNode@%p", static_cast<const void*>(m_highlighted));
				Text("Centre: (%.4e m, %.4e m)", centre.x, centre.y);
				Text("Side length: %.4e m", len);
				Text("Centre of mass: (%.4e m, %.4e m)", centre_mass.x, centre_mass.y);
				Text("Level: %zu", m_highlighted->getLevel());
				Text("Bodies contained: %zu", m_highlighted->getNumBodies());
				Spacing();
			}
			else
				Text("Mouse over a tree node to see statistics");
		}

		if (CollapsingHeader("Body editor"))
		{
			/*    This is naughty   */
			auto state = const_cast<ParticleState*>(reinterpret_cast<ParticleState const *>(m_sim->m_int_ptr->getStateVector()));
			auto aux_state = const_cast<ParticleAuxState*>(m_sim->m_mod_ptr->getAuxState());

			auto static idx = 0;
			auto static pos = &state[0].pos;
			auto static vel = &state[0].vel;
			auto static mass = &aux_state[0].mass;
			auto static draw_line = false;
			auto static energy = 0.0;

			InputInt("Index", &idx);
			if (idx < 0)
				idx = 0;
			if (idx >= m_sim->m_mod_ptr->getNumBodies())
				idx = static_cast<int>(m_sim->m_mod_ptr->getNumBodies() - 1);

			pos = &state[idx].pos;
			vel = &state[idx].vel;
			mass = &aux_state[idx].mass;

			InputDoubleScientific2("Position", reinterpret_cast<double*>(pos));
			SameLine();
			auto start = GetCursorScreenPos();
			Checkbox("Show", &draw_line);

			if (draw_line)
			{
				auto draw_list = GetWindowDrawList();
				auto end = Vector2f{ Display::worldToScreenX(pos->x), Display::worldToScreenY(pos->y) };
				draw_list->PushClipRectFullScreen();
				auto sz = 5.f * Display::bodyScalingFunc(Display::screen_scale);
				auto tl = ImVec2{ end + Vector2f{ -sz, -sz } };
				auto tr = ImVec2{ end + Vector2f{  sz, -sz } };
				auto bl = ImVec2{ end + Vector2f{ -sz,  sz } };
				auto br = ImVec2{ end + Vector2f{  sz,  sz } };
				draw_list->AddQuad(tl, tr, br, bl, IM_COL32_WHITE);
				draw_list->AddLine(start, tl, IM_COL32_WHITE);
				draw_list->PopClipRect();
			}

			InputDoubleScientific2("Velocity", reinterpret_cast<double*>(vel));

			if (InputDoubleScientific("Mass", mass))
			{
				// need to update cached radius
				m_body_mgr.setDirty();
			}

			if (Button("Energy"))
			{
				auto ke = 0.5 * (*mass) * vel->mag_sq();
				auto pe = 0.0;
				for (auto i = 0; i < m_sim->m_mod_ptr->getNumBodies(); i++)
				{
					if (i == idx)
						continue;
					auto rel_pos_mag = (*pos - state[i].pos).mag();
					pe += -aux_state[i].mass * (*mass) * Constants::G / rel_pos_mag;
				}
				energy = pe + ke;
			}
			SameLine();
			Text("%.3g", energy);
			Spacing();
		}

		/*Text("Screen scale: %f", Display::screen_scale);
		Text("Body scale: %f", Display::bodyScalingFunc(Display::screen_scale));
		SliderFloat("Scaling crossover", &Display::scaling_cross, 0.001, 1, "%.4f", 10);
		SliderFloat("Scaling smoothing", &Display::scaling_smooth, 0.001, 1, "%.4f", 10);

		float(*scl)(void*, int) = [](void*, int i)
		{
			return static_cast<float>(Display::bodyScalingFunc(i * 0.01f));
		};
		PlotLines("Scaling function", scl, nullptr, 100, 0, nullptr, 0.5f, 5.f, { 0, 80 });*/

		/*auto static show_ellipses_l = false;
		auto static show_ellipses_r = false;

		auto constexpr num = 100;
		auto constexpr galaxy_rad = 13000 * Constants::PARSEC;
		auto constexpr dr = galaxy_rad / num;
		auto static delta_angle = 2 * Constants::PI * Constants::PARSEC / galaxy_rad;

		InputDouble("Angle increment", &delta_angle);
		Text("Show ellipses");
		SameLine();
		Checkbox("Left focus", &show_ellipses_l);
		SameLine();
		Checkbox("Right focus", &show_ellipses_r);

		for (auto i = 0; i < num; i++)
		{
			auto r = (i + 1) * dr;
			if (show_ellipses_l)
				drawEllipse(r, eccentricity(r), r / Constants::PARSEC * delta_angle);
			if (show_ellipses_r)
				drawEllipse(r, eccentricity(r), r / Constants::PARSEC * delta_angle - Constants::PI);
		}*/

		End();

		//ShowTestWindow();

		auto constexpr scale_overlay_width = 300.f;
		auto constexpr scale_overlay_height = 50.f;
		auto const scale_overlay_pos = Display::screen_size - Vector2f{ scale_overlay_width + 10, scale_overlay_height + 10 };

		// get world length that fits in overlay
		auto const max_wl = Display::screenToWorldLength(scale_overlay_width);
		// find nearest power of ten
		auto const log_wl_pc = static_cast<int>(floor(log10(max_wl / Constants::PARSEC)));
		// corresponding number of parsecs
		auto const num_pc = pow(10.0, log_wl_pc);
		// get corresponding screen length
		auto const scale_sl = Display::worldToScreenLength(num_pc * Constants::PARSEC);

		SetNextWindowPos(scale_overlay_pos);
		SetNextWindowSize({ scale_overlay_width, scale_overlay_height });

		Begin("Scale", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		auto draw_list = GetWindowDrawList();
		auto cursor_pos = GetCursorScreenPos();
		auto begin = ImVec2{ scale_overlay_pos.x + (scale_overlay_width - scale_sl) / 2.f, cursor_pos.y };
		auto end = ImVec2{ begin.x + scale_sl, begin.y };
		draw_list->AddLine(begin, end, IM_COL32_WHITE, 3);
		Spacing();
		cursor_pos = GetCursorPos();

		char scale_text[32];
#ifdef SAFE_STRFN
		sprintf_s(scale_text, "%.0f pc", num_pc);
#else
		sprintf(scale_text, "%.0f pc", num_pc);
#endif
		auto text_len = CalcTextSize(scale_text).x;
		SetCursorPos({ GetWindowWidth() / 2.f - text_len / 2.f, cursor_pos.y });
		Text(scale_text);
		End();
	}

	/*void drawEllipse(double const a, double const ecc, double const beta)
	{
		using namespace ImGui;

		constexpr auto N_PTS = 100;
		constexpr auto delta_ang = 2 * Constants::PI / N_PTS;

		auto damp = 40;
		auto nump = 2;

		ImVec2 points[N_PTS];

		auto draw_list = GetWindowDrawList();
		draw_list->PushClipRectFullScreen();
		for (auto i = 0; i < N_PTS; i++)
		{
			auto alpha = i * delta_ang;
			auto r = a * (1 - ecc * ecc) / (1 + ecc * cos(alpha - beta));

			points[i] = ImVec2{ Display::worldToScreenX(r * cos(alpha)), Display::worldToScreenY(r * sin(alpha)) };
		}
		draw_list->AddPolyline(points, N_PTS, IM_COL32_WHITE, true, 1, true);
		draw_list->PopClipRect();
	}

	double eccentricity(double const r)
	{
		auto rad = r / Constants::PARSEC;

		auto galaxy_rad = 13000;
		auto core_rad = 0.3 * galaxy_rad;
		auto s_ECC_CORE = 0.35;
		auto s_ECC_DISK = 0.2;

		if (rad < core_rad)
			return rad / core_rad * s_ECC_CORE;

		if (rad < galaxy_rad)
			return s_ECC_CORE + (rad - core_rad) / (galaxy_rad - core_rad) * (s_ECC_DISK - s_ECC_CORE);

		if (rad < 2 * galaxy_rad)
			return s_ECC_DISK + (rad - galaxy_rad) / galaxy_rad * (0.0 - s_ECC_DISK);

		return 0;
	}*/

	void RunState::draw(sf::Time const dt)
	{
		timings[Timings::RENDER_START] = Clock::now();
		if (m_flags.view_centre)
		{
			auto com = m_sim->m_mod_ptr->getCentreMass();
			auto com_screen = Vector2f{ Display::worldToScreenX(com.x), Display::worldToScreenY(com.y) };
			Display::screen_offset += com_screen - 0.5f * Display::screen_size;
		}

		m_sim->m_window.setView(m_main_view);

		if (m_flags.show_bodies)
		{
			m_sim->m_window.draw(m_body_mgr);
		}

		if (m_flags.tree_exists && m_flags.show_grid)
		{
			m_sim->m_window.draw(m_quad_mgr);
		}

		if (m_flags.show_trails)
		{
			m_sim->m_window.draw(m_trail_mgr);
		}

		m_sim->m_window.setView(m_gui_view);

		ImGui::Render();
		timings[Timings::RENDER_END] = Clock::now();
	}

	void RunState::handleInput()
	{
		sf::Event event;
		sf::Vector2i static prev_mouse_pos, new_mouse_pos;
		while (m_sim->m_window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (!ImGui::GetIO().WantCaptureMouse)
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					m_sim->m_window.close();
					break;
				}
				case sf::Event::Resized:
				{
					Display::screen_size = { static_cast<float>(event.size.width), static_cast<float>(event.size.height) };
					Display::aspect_ratio = static_cast<float>(event.size.width) / static_cast<float>(event.size.height);
					m_gui_view.setSize(Display::screen_size);
					m_gui_view.setCenter(Display::screen_size * 0.5f);
					m_main_view.setSize(Display::screen_size);
					m_main_view.setCenter(Display::screen_size * 0.5f);
					break;
				}
				case sf::Event::KeyPressed:
				{
					if (event.key.code == sf::Keyboard::Space)
					{
						m_flags.running = !m_flags.running;
					}
					else if (event.key.code == sf::Keyboard::C)
					{
						m_flags.view_centre = !m_flags.view_centre;
					}
					else if (event.key.code == sf::Keyboard::G)
					{
						m_flags.show_grid = !m_flags.show_grid;
					}
					else if (event.key.code == sf::Keyboard::M && m_flags.show_grid)
					{
						m_flags.grid_mode_complete = !m_flags.grid_mode_complete;
					}
					else if (event.key.code == sf::Keyboard::B)
					{
						m_flags.show_bodies = !m_flags.show_bodies;
					}
					else if (event.key.code == sf::Keyboard::T && m_flags.show_bodies)
					{
						m_flags.show_trails = !m_flags.show_trails;
						m_trail_mgr.reset();
					}
					else if (event.key.code == sf::Keyboard::R)
					{
						m_flags.view_centre = false;
						Display::screen_scale = 1;
						Display::screen_offset = { 0, 0 };
					}
					else if (event.key.code == sf::Keyboard::Escape)
					{
						m_sim->popState();
					}
					break;
				}
				case sf::Event::MouseWheelScrolled:
				{
					auto old_scale = Display::screen_scale;
					// calculate new display scale
					Display::screen_scale *= pow(2.f, -0.1f * event.mouseWheelScroll.delta);
					// shift offsets to keep view centred on same world position
					Display::screen_offset *= old_scale / Display::screen_scale;
					break;
				}
				case sf::Event::MouseMoved:
				{
					// if dragging, update pos
					if (m_flags.view_dragging)
					{
						prev_mouse_pos = new_mouse_pos;
						new_mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
						Display::screen_offset += Vector2f{ new_mouse_pos - prev_mouse_pos };
					}
					break;
				}
				case sf::Event::MouseButtonPressed:
				{
					// if not dragging, start
					if (!m_flags.view_dragging)
					{
						m_flags.view_centre = false;
						m_flags.view_dragging = true;
						new_mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
					}
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					// if dragging, stop
					if (m_flags.view_dragging)
					{
						m_flags.view_dragging = false;
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}