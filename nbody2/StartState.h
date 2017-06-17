#ifndef START_STATE_H
#define START_STATE_H

#include "BodyDistributor.h"
#include "IColourer.h"
#include "IModel.h"
#include "IIntegrator.h"
#include "Sim.h"
#include "IState.h"

#include "imgui.h"

#include <SFML/Graphics.hpp>

#include <vector>

namespace nbody
{
	struct BodyGroupProperties;

	enum class MenuState
	{
		INITIAL,
		LOAD_EXISTING,
		CREATE_NEW
	};

	using ComboCallback = bool(*)(void*, int, char const**);

	template<typename T>
	ComboCallback callback = [](void * data, int idx, const char ** out_text)
	{
		auto& array = *static_cast<T*>(data);
		if (idx < 0 || idx >= static_cast<int>(array.size()))
		{
			return false;
		}
		else
		{
			*out_text = array[idx].name;
			return true;
		}
	};

	class StartState : public IState
	{
	public:
		explicit StartState(Sim * simIn);
		virtual ~StartState() = default;

		void draw(sf::Time const dt) override;
		void update(sf::Time const dt) override;
		void handleInput() override;
	private:
		bool checkRun(std::string & result_message) const;

		void makeInitialWindow();
		void makeLoadWindow();
		void makeGenerateWindow();
		void makeMassPopup(size_t const idx) const;
		void makeCentralMassPopup(size_t const idx) const;
		void makePosVelPopup(size_t const idx) const;
		void makeRadiusPopup(size_t const idx) const;
		void makeColourPopup(size_t const idx) const;
		void makeSavePopup();

		void saveSettings(char const* filename);
		bool loadSettings(char const* filename);

		bool m_do_run;

		sf::View m_view;

		MenuState m_menu_state;
		bool m_l1_modal_open, m_l2_modal_open;
		ImGuiWindowFlags m_window_flags;
		ImGuiStyle& m_style;

		// Sets of parameters for BodyGroups 
		// References those held in Sim
		std::vector<BodyGroupProperties> & m_bg_props;
		// Simulation-wide parameters
		// References those held in Sim
		SimProperties & m_sim_props;

		// Error message from file load operations
		std::string m_err_string;

		// Callback functions for drop-down menus
		ComboCallback m_getDistributorName = callback<DPArray>;
		ComboCallback m_getColourerName = callback<CPArray>;
		ComboCallback m_getIntegratorName = callback <IntArray>;
		ComboCallback m_getModelName = callback<ModArray>;
	};

	namespace fileio
	{
		char constexpr FILE_HEADER[] = "nb_settings";
		char constexpr VERSION[] = "v6";
		char constexpr GLOBAL_HEADER[] = "global";
		char constexpr ITEM_HEADER[] = "bgprop";
		char constexpr SEP[] = "__";
		size_t constexpr SIZE_FH = sizeof(FILE_HEADER);
		size_t constexpr SIZE_VER = sizeof(VERSION);
		size_t constexpr SIZE_GH = sizeof(GLOBAL_HEADER);
		size_t constexpr SIZE_IH = sizeof(ITEM_HEADER);
	}
}

#endif // START_STATE_H
