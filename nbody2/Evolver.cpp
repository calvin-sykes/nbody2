#include "BHTree.h"
#include "Body2d.h"
#include "Constants.h"
#include "Evolver.h"
#include "Quad.h"
#include "RunState.h"
#include "Sim.h"
#include "Vector.h"

#include <vector>

namespace nbody
{
	void BruteForceEvolver::step(std::vector<Body2d> & bodies, RunState * context)
	{
#pragma omp parallel for schedule(static)
		for (int i = 0; i < bodies.size(); i++)
		{
			for (int j = 0; j < bodies.size(); j++)
			{
				bodies[i].addAccel(bodies[j]);
				bodies[j].addAccel(bodies[i]);
			}
		}
#pragma omp parallel for schedule(static)
		for (int i = 0; i < bodies.size(); i++)
		{
			bodies[i].update(Constants::TIMESTEP);
		}
	}

	void BarnesHutEvolver::step(std::vector<Body2d> & bodies, RunState * context)
	{
		// create root quadrant of tree
		Quad root(context->com.x, context->com.y, 10 * Constants::RADIUS);

		// reconstruct BH tree if needed
		if (context->tree_old || context->current_show_grid != context->show_grid)
		{
			context->tree_ptr = buildTreeThreaded(bodies, root);
			context->com = context->tree_ptr->getPCoM();
			context->tree_old = false;
			context->current_show_grid = context->show_grid;
		}

		if (context->running)
		{
			// advance bodies
#pragma omp parallel for schedule(static)
			for (int i = 0; i < bodies.size(); i++)
			{
				if (root.contains(bodies[i].getPos()))
				{
					context->tree_ptr->updateAccel(bodies[i]);
					bodies[i].update(Constants::TIMESTEP);
					bodies[i].resetAccel();
				}
			}
			context->tree_old = true;
		}
	}

	BHTree * nbody::BarnesHutEvolver::buildTreeThreaded(std::vector<Body2d> const & bodies, Quad const & root)
	{
		BHTree * nwt, *net, *swt, *set;
#pragma omp parallel sections
		{
#pragma omp section
			{
				nwt = buildTree(bodies, root.makeDaughter(NW));
			}
#pragma omp section
			{
				net = buildTree(bodies, root.makeDaughter(NE));
			}
#pragma omp section
			{
				swt = buildTree(bodies, root.makeDaughter(SW));
			}
#pragma omp section
			{
				set = buildTree(bodies, root.makeDaughter(SE));
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
}