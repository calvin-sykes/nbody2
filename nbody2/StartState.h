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
	using DPArray = std::array<DistributorProperties, static_cast<size_t>(DistributorType::N_DISTRIBUTIONS)>;
	using CPArray = std::array<ColourerProperties, static_cast<size_t>(ColourerType::N_TYPES)>;
	using IntArray = std::array<IntegratorProperties, static_cast<size_t>(IntegratorType::N_INTEGRATORS)>;
	using ModArray = std::array<ModelProperties, static_cast<size_t>(ModelType::N_MODELS)>;

	struct BodyGroupProperties;

	enum class MenuState
	{
		INITIAL,
		LOAD_EXISTING,
		CREATE_NEW
	};

	using ComboCallback = bool(*)(void*, int, char const**);

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
		void makeColourPopup(size_t const idx);
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

		// Info on BodyDistributors
		DPArray m_dist_infos = { { 
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

		ComboCallback m_getDistributorName = [](void * data, int idx, const char ** out_text)
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
		};

		CPArray m_colour_infos = { {
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

		ComboCallback m_getColourerName = [](void * data, int idx, const char ** out_text)
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
		};

		IntArray m_integrator_infos = { {
			{
				IntegratorType::EULER,
				"Euler"
			},
			{
				IntegratorType::MODIFIED_EULER,
				"Modified Euler"
			}
		} };

		ComboCallback m_getIntegratorName = [](void * data, int idx, const char ** out_text)
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
		};

		ModArray m_model_infos = { {
			{
				ModelType::BRUTE_FORCE,
				"Brute-force",
				"Forces between every pair of bodies are calculated directly"
			},
			{
				ModelType::BARNES_HUT,
				"Barnes-Hut",
				"Long-range forces are approximated using a Barnes-Hut tree"
			}
			} };

		ComboCallback m_getModelName = [](void * data, int idx, const char ** out_text)
		{
			auto& array = *static_cast<ModArray*>(data);
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
