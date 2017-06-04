#ifndef SIM_H
#define SIM_H

#include "AssetManager.h"
#include "Body2d.h"
#include "BodyGroupProperties.h"
#include "IModel.h"
#include "IIntegrator.h"

#include <SFML/Graphics.hpp>

#include <iterator>
#include <stack>
#include <vector>

namespace nbody
{
	class SimState;

	// Apend a vector rvalue to a secoond vector, using move semantics
	//template <typename T>
	//typename std::vector<T>::iterator append(std::vector<T>&& src, std::vector<T>& dest)
	//{
	//	typename std::vector<T>::iterator result;

	//	if (dest.empty())
	//	{
	//		dest = std::move(src);
	//		result = std::begin(dest);
	//	}
	//	else
	//	{
	//		result = dest.insert(std::end(dest),
	//			std::make_move_iterator(std::begin(src)),
	//			std::make_move_iterator(std::end(src)));
	//	}

	//	src.clear();
	//	src.shrink_to_fit();

	//	return result;
	//}

	struct SimProperties
	{
		SimProperties()
			: timestep(-1.),
			int_type(IntegratorType::INVALID),
			mod_type(ModelType::INVALID),
			bg_props()
			{}

		double timestep;
		IntegratorType int_type;
		ModelType mod_type;
		std::vector<BodyGroupProperties> bg_props;
		size_t n_bodies;
	};

	enum class PendingStateOp
	{
		NONE,
		PUSH,
		POP,
		CHANGE
	};

	struct PendingState
	{
		PendingState() : op(PendingStateOp::NONE), state(nullptr) {}

		PendingStateOp op;
		SimState * state;
	};

	class Sim
	{
	public:
		Sim();
		~Sim();

		void pushState(SimState* state);
		void popState();
		void changeState(SimState* state);
		SimState* peekState();

		void loadTextures();
		void loadObjects();
		void loadFonts();

		void setProperties(SimProperties const&);
		void simLoop();

		AssetManager m_asset_mgr;

		SimProperties m_sim_props;

		std::unique_ptr<IIntegrator> m_int_ptr;
		std::unique_ptr<IModel> m_mod_ptr;
		double m_step;

		sf::RenderWindow m_window;	
		sf::Sprite m_background;

	private:
		std::stack<SimState*> m_states;
		PendingState m_pending;

		void handlePendingOp();
	};
}
#endif // SIM_H