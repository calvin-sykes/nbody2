#ifndef START_STATE_H
#define START_STATE_H

#include "SimState.h"
#include "BodyDistributor.h"

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

	struct DistributorProperties
	{
		DistributorType const type;
		char const* name;
		char const* tooltip;
		bool const has_central_mass;
	};

	enum class ColourerType
	{
		SOLID,
		VELOCITY,
		N_TYPES,
		INVALID = -1
	};

	struct ColourerProperties
	{
		ColourerType const type;

	};

	using DPArray = std::array<DistributorProperties, static_cast<size_t>(DistributorType::N_DISTRIBUTIONS)>;
	using CPArray = std::array<ColourerProperties, static_cast<size_t>(ColourerType::N_TYPES)>;

	struct BodyGroupProperties
	{
		BodyGroupProperties() :
			dist(DistributorType::INVALID), N(0), min_mass(0), max_mass(0),
			has_central_mass(false), central_mass(0), pos(), vel() {}

		int N;
		DistributorType dist;
		double min_mass;
		double max_mass;
		bool has_central_mass;
		double central_mass;
		Vector2d pos, vel;
	};

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
		// Temporary storage variables
		std::vector<char> tmp_use_relative_coords;

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
		// Get the .name field of a DistributorProperties object
		// for displaying in a ComboBox
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
	};
}

#endif // START_STATE_H
