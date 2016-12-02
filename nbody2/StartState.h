#ifndef START_STATE_H
#define START_STATE_H

#include "BodyDistributor.h"
#include "Colourer.h"
#include "Evolver.h"
#include "Integrator.h"
#include "Sim.h"
#include "SimState.h"

#include "imgui.h"

#include <SFML/Graphics.hpp>

#include <fstream>
#include <string>
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

	struct TempColArray
	{
		float cols[static_cast<size_t>(ColourerType::N_TYPES)][MAX_COLS_PER_COLOURER][3];
	};

	using DPArray = std::array<DistributorProperties, static_cast<size_t>(DistributorType::N_DISTRIBUTIONS)>;
	using CPArray = std::array<ColourerProperties, static_cast<size_t>(ColourerType::N_TYPES)>;
	using IntArray = std::array<IntegratorProperties, static_cast<size_t>(IntegratorType::N_INTEGRATORS)>;
	using EvlArray = std::array<EvolverProperties, static_cast<size_t>(EvolverType::N_METHODS)>;
	using ComboCallback = bool(*)(void*, int, char const**);

	class StartState : public SimState
	{
	public:
		StartState(Sim * simIn);

		virtual void draw(sf::Time const dt);
		virtual void update(sf::Time const dt);
		virtual void handleInput();
	private:
		void run();

		void makeInitialWindow();
		void makeLoadWindow();
		void makeGenerateWindow();
		void makeMassPopup(size_t const idx);
		void makeCentralMassPopup(size_t const idx);
		void makePosVelPopup(size_t const idx);
		void makeRadiusPopup(size_t const idx);
		void makeColourPopup(size_t const idx);
		void makeSavePopup();

		void saveSettings(char const* filename);
		bool loadSettings(char const* filename);

		sf::View view;

		MenuState menu_state;
		bool l1_modal_is_open, l2_modal_is_open;
		ImGuiWindowFlags window_flags;
		ImGuiStyle& style;

		// Sets of parameters for BodyGroups 
		std::vector<BodyGroupProperties> bg_props;
		// Simulation-wide parameters
		SimProperties sim_props;
		// Temporary storage for imgui 0..1 colours
		std::vector<TempColArray> tmp_cols;

		// Info on BodyDistributors
		DPArray static constexpr dist_infos = { {
			{
				DistributorType::EXPONENTIAL,
				"Exponential",
				"The density of bodies falls off exponentially with distance from a large central mass.",
				true
			},
			{
				DistributorType::ISOTHERMAL,
				"Isothermal",
				"The density of bodies falls off with the square of the distance from a large central mass.",
				true
			},
			{
				DistributorType::PLUMMER,
				"Plummer",
				"Bodies are distributed according to the Plummer globular cluster model.",
				false
			}
		} };

		ComboCallback getDistributorName{ [](void * data, int idx, const char ** out_text)
		{
			auto& array = *static_cast<DPArray*>(data);
			if (idx < 0 || idx >= static_cast<int>(array.size()))
			{
				return false;
			}
			else
			{
				*out_text = array[idx].name;
				return true;
			}
		} };

		CPArray static constexpr colour_infos = { {
			{
				ColourerType::SOLID,
				"Single",
				"All bodies in this group are coloured the same",
				1
			},
			{
				ColourerType::VELOCITY,
				"Velocity",
				"Bodies in this group are coloured according to their velocity",
				2
			}
		} };

		ComboCallback getColourerName{ [](void * data, int idx, const char ** out_text)
		{
			auto& array = *static_cast<CPArray*>(data);
			if (idx < 0 || idx >= static_cast<int>(array.size()))
			{
				return false;
			}
			else
			{
				*out_text = array[idx].name;
				return true;
			}
		} };

		IntArray integrator_infos = { {
			{
				IntegratorType::EULER,
				"Euler"
			}
		} };

		ComboCallback getIntegratorName{ [](void * data, int idx, const char ** out_text)
		{
			auto& array = *static_cast<IntArray*>(data);
			if (idx < 0 || idx >= static_cast<int>(array.size()))
			{
				return false;
			}
			else
			{
				*out_text = array[idx].name;
				return true;
			}
		} };

		EvlArray evolve_infos = { {
			{
				EvolverType::BRUTE_FORCE,
				"Brute-force",
				"Forces between every pair of bodies are calculated directly"
			},
			{
				EvolverType::BARNES_HUT,
				"Barnes-Hut",
				"Long-range forces are approximated using a Barnes-Hut tree"
			}
		} };

		ComboCallback getEvolveName{ [](void * data, int idx, const char ** out_text)
		{
			auto& array = *static_cast<EvlArray*>(data);
			if (idx < 0 || idx >= static_cast<int>(array.size()))
			{
				return false;
			}
			else
			{
				*out_text = array[idx].name;
				return true;
			}
		} };
	};

	namespace fileio
	{
		char constexpr FILE_HEADER[] = "nb_settings";
		char constexpr VERSION[] = "v2";
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
