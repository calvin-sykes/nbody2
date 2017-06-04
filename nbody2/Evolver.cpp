#include "BHTree.h"
#include "Body2d.h"
#include "Constants.h"
#include "Evolver.h"
#include "Quad.h"
#include "RunState.h"
#include "Sim.h"
#include "Vector.h"

#include <vector>

/*namespace nbody
{
	void BruteForceEvolver::calcStep(std::vector<Body2d> & bodies, Vector2d & com, BHTree *& tree_ptr, Flags & flags)
	{
		com = { 0, 0 };
		tree_ptr = nullptr;
		flags.tree_old = false;
#pragma omp parallel for schedule(static)
		for (int i = 0; i < bodies.size(); i++)
		{
			for (int j = i + 1; j < bodies.size(); j++)
			{
				bodies[i].addAccel(bodies[j]);
				bodies[j].addAccel(bodies[i]);
			}
		}
	}

	void BruteForceEvolver::advanceStep(std::vector<Body2d> & bodies, double const dt, Vector2d & com, BHTree *& tree_ptr, Flags & flags)
	{
#pragma omp parallel for schedule(static)
		for (int i = 0; i < bodies.size(); i++)
		{
			bodies[i].update(dt);
			bodies[i].resetAccel();
		}
	}

	void BarnesHutEvolver::calcStep(std::vector<Body2d> & bodies, Vector2d & com, BHTree *& tree_ptr, Flags & flags)
	{
		// create root quadrant of tree
		root = Quad(com.x, com.y, 10 * Constants::RADIUS);

			tree_ptr = buildTreeThreaded(bodies);
			com = tree_ptr->getPCoM();
			flags.tree_old = false;
			flags.current_show_grid = flags.show_grid;
	}

	void BarnesHutEvolver::advanceStep(std::vector<Body2d> & bodies, double const dt, Vector2d & com, BHTree *& tree_ptr, Flags & flags)
	{
#pragma omp parallel for schedule(static)
		for (int i = 0; i < bodies.size(); i++)
		{
			if (root.contains(bodies[i].getPos()))
			{
				tree_ptr->updateAccel(bodies[i]);
				bodies[i].update(dt);
				bodies[i].resetAccel();
			}
		}
		flags.tree_old = true;
	}

	BHTree * nbody::BarnesHutEvolver::buildTreeThreaded(std::vector<Body2d> const & bodies)
	{
		BHTree * nwt, *net, *swt, *set;
#pragma omp parallel sections
		{
#pragma omp section
			{
				nwt = buildTree(bodies, root.createDaughter(Daughter::NW));
			}
#pragma omp section
			{
				net = buildTree(bodies, root.createDaughter(Daughter::NE));
			}
#pragma omp section
			{
				swt = buildTree(bodies, root.createDaughter(Daughter::SW));
			}
#pragma omp section
			{
				set = buildTree(bodies, root.createDaughter(Daughter::SE));
			}
		}
		return new BHTree(nwt, net, swt, set);
	}

	BHTree * nbody::BarnesHutEvolver::buildTree(std::vector<Body2d> const & bodies, Quad const & root)
	{
		auto tree_ptr = new BHTree(root, 0);
		for (auto& b : bodies)
			tree_ptr->insert(&b);
		return tree_ptr;
	}
}*/