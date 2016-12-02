#ifndef EVOLVER_H
#define EVOLVER_H

namespace nbody
{
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

	};

	class BruteForceEvolver : public Evolver
	{

	};

	class BarnesHutEvolver : public Evolver
	{

	};

}

#endif // EVOLVER_H
