#ifndef EVOLVER_H
#define EVOLVER_H

#include "Body2d.h"

#include <vector>

namespace nbody
{
	class BHTree;
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
		virtual void step(std::vector<Body2d> & bodies, RunState * context) = 0;
	};

	class BruteForceEvolver : public Evolver
	{
	public:
		virtual void step(std::vector<Body2d> & bodies, RunState * context);
	};

	class BarnesHutEvolver : public Evolver
	{
	public:
		virtual void step(std::vector<Body2d> & bodies, RunState * context);
	private:
		BHTree * buildTreeThreaded(std::vector<Body2d> const& bodies, Quad const& root);
		BHTree * buildTree(std::vector<Body2d> const& bodies, Quad const& root);
	};

}

#endif // EVOLVER_H
