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
		DistributorType type;
		const char * name;
		const char * tooltip;
		bool has_central_mass;
	};

	using DPArray = std::array<DistributorProperties, static_cast<size_t>(DistributorType::N_DISTRIBUTIONS)>;

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

		sf::View view;

		MenuState menu_state;
		bool l1_modal_is_open, l2_modal_is_open;
		ImGuiWindowFlags window_flags;
		ImGuiStyle& style;
		// Sets of parameters for BodyGroups 
		std::vector<BodyGroupProperties> bg_props;
		// Temporary storage variables
		std::vector<float> tmp_min_mass;
		std::vector<float> tmp_max_mass;
		std::vector<float> tmp_central_mass;
		std::vector<DistributorType> tmp_dist_type;
		std::vector<Vector2d> tmp_pos;
		std::vector<Vector2d> tmp_vel;
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
