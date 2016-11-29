#ifndef START_STATE_H
#define START_STATE_H

#include "SimState.h"
#include "BodyDistributor.h"
#include "BodyGroupProperties.h"

#include <vector>

#include <SFML/Graphics.hpp>

namespace nbody
{
	enum class MenuState
	{
		INITIAL,
		LOAD_EXISTING,
		CREATE_NEW
	};

	using DPArray = std::array<DistributorProperties, static_cast<size_t>(DistributorType::N_DISTRIBUTIONS)>;
	using CPArray = std::array<ColourerProperties, static_cast<size_t>(ColourerType::N_TYPES)>;
	using IntArray = std::array<IntegratorProperties, static_cast<size_t>(IntegratorType::N_INTEGRATORS)>;
	using EvlArray = std::array<EvolveProperties, static_cast<size_t>(EvolveType::N_METHODS)>;
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

		void InitialWindow();
		void LoadWindow();
		void GenerateWindow();
		void MassPopup(size_t const idx);
		void CentralMassPopup(size_t const idx);
		void PosVelPopup(size_t const idx);
		void ColourPopup(size_t const idx);

		sf::View view;

		MenuState menu_state;
		bool l1_modal_is_open, l2_modal_is_open;
		ImGuiWindowFlags window_flags;
		ImGuiStyle& style;

		// Sets of parameters for BodyGroups 
		std::vector<BodyGroupProperties> bg_props;
		// SImulation-wide parameters
		SimProperties sim_props;
		// Temporary storage variables
		std::vector<char> tmp_use_relative_coords;
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
				EvolveType::BRUTE_FORCE,
				"Brute-force",
				"Forces between every pair of bodies are calculated directly"
			},
			{
				EvolveType::BARNES_HUT,
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
}

#endif // START_STATE_H
