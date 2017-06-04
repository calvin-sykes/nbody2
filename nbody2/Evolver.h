#ifndef EVOLVER_H
#define EVOLVER_H

#include "Body2d.h"
#include "Quad.h"

#include <vector>

/*namespace nbody
{
	class BHTree;
	struct Flags;
	class Quad;
	class RunState;

	enum class EvolverType
	{
		BRUTE_FORCE,
		BARNES_HUT,
		N_METHODS,
		INVALID = -1
	};

	struct EvolverProperties
	{
		EvolverType type;
		char const* name;
		char const* tooltip;
	};

	class Evolver
	{
	public:
		Evolver(bool has_tree) : has_tree(has_tree) {};

		virtual void calcStep(std::vector<Body2d> & bodies, Vector2d & com, BHTree *& tree_ptr, Flags & flags) = 0;
		virtual void advanceStep(std::vector<Body2d> & bodies, double const dt, Vector2d & com, BHTree *& tree_ptr, Flags & flags) = 0;
	public:
		bool const has_tree;
	};

	class BruteForceEvolver : public Evolver
	{
	public:
		BruteForceEvolver() : Evolver(false) {};

		virtual void calcStep(std::vector<Body2d> & bodies, Vector2d & com, BHTree *& tree_ptr, Flags & flags);
		virtual void advanceStep(std::vector<Body2d> & bodies, double const dt, Vector2d & com, BHTree *& tree_ptr, Flags & flags);
	};

	class BarnesHutEvolver : public Evolver
	{
	public:
		BarnesHutEvolver() : Evolver(true) {};

		virtual void calcStep(std::vector<Body2d> & bodies, Vector2d & com, BHTree *& tree_ptr, Flags & flags);
		virtual void advanceStep(std::vector<Body2d> & bodies, double const dt, Vector2d & com, BHTree *& tree_ptr, Flags & flags);
	private:
		BHTree * buildTreeThreaded(std::vector<Body2d> const& bodies);
		BHTree * buildTree(std::vector<Body2d> const& bodies, Quad const& root);

		Quad root;
	};

}*/

#endif // EVOLVER_H
