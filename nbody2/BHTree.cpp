#include "BHTree.h"
#include "Constants.h"

namespace nbody
{
	size_t BHTree::max_level = 0;

	BHTree::BHTree(const Quad& qIn, const size_t levelIn) :
		q(qIn), n(0), level(levelIn), mass(0), com(), b(nullptr), daughters{nullptr}
	{
		if (level > max_level) max_level = level;
	}

	BHTree::BHTree(pTree nwIn, pTree neIn, pTree swIn, pTree seIn) :
		com(), n(0), mass(0), b(nullptr), daughters{swIn, seIn, nwIn, neIn }
	{
		// deduce dimensions from daughters
		double length;
		try
		{
			// require daughters to be of equal size
			for (size_t i = 0; i < N_DAUGHTERS; i++)
			{
				if (daughters[NW]->q.getLength() != daughters[i]->q.getLength())
					throw std::invalid_argument("Constructing BHTree: daughter quads have different lengths");
			}
			// ok
			length = daughters[0]->q.getLength();

			// require no gaps between daughters
			if (!((daughters[NW]->q.getXmid() + length / 2) - (daughters[NE]->q.getXmid() - length / 2) < 0.0001 * RAD)
				&& !((daughters[NW]->q.getYmid() - length / 2) - (daughters[SW]->q.getYmid() + length / 2) < 0.0001 * RAD))
			{
				throw std::invalid_argument("Constructing BHTree: daughter quads not space-filling");
			}

			// require daughters to be of same level
			for (size_t i = 0; i < N_DAUGHTERS; i++)
			{
				if (daughters[NW]->level != daughters[i]->level)
					throw std::invalid_argument("Constructing BHTree: daughter quads have different levels");
			}
			// ok
			level = daughters[NW]->level - 1;
		}
		catch (std::invalid_argument except)
		{
			throw except;
		}

		q = Quad(daughters[NW]->q.getXmid() + length / 2, daughters[NW]->q.getYmid() - length / 2, 2 * length);

		// if body is null, prune the subtree
		// NB: reference to pointer so that original pointer is set to nullptr
		for (auto& d : daughters)
		{
			if (d->n == 0)
			{
				delete d;
				d = nullptr;
			}
			else
			{
				com = (com * mass + d->com * d->mass) * (1.0 / (mass + d->mass));
				mass += d->mass;
				n += d->n;
			}
		}
	}

	BHTree::~BHTree()
	{
		for (auto d : daughters)
		{
			if (d != nullptr)
				delete d;
		}
	}

	void BHTree::insert(pcBody const bIn)
	{
		// which daughter, if any, would contrain this body?
		auto which_daughter = q.whichDaughter(bIn->getPos());
		// If tree doesn't contain body, do nothing
		if (which_daughter == NONE)
		{
			return;
		}
		// If this tree already contains bodies
		if (n > 0)
		{
			// if tree only contains one body it is currently a leaf
			// now daughter trees need to be created
			if (n == 1)
			{
				// recursively add current body to correct quadrant
				auto current_daughter = q.whichDaughter(b->getPos());
				daughters[current_daughter] = new BHTree(q.makeDaughter(current_daughter), level + 1);
				daughters[current_daughter]->insert(b);
				// tree is no longer leaf
				b = nullptr;
			}
			// create quadrant for new body if it does not exist
			if (daughters[which_daughter] == nullptr)
				daughters[which_daughter] = new BHTree(q.makeDaughter(which_daughter), level + 1);
			// add new body
			daughters[which_daughter]->insert(bIn);
		}
		else // n == 0
		{
			// store body in this tree leaf
			b = bIn;
		}
		// update centre of mass
		com = (com * mass + bIn->getPos() * bIn->getMass()) * (1.0 / (mass + bIn->getMass()));
		// increment body counter
		n++;
		// update mass
		mass += bIn->getMass();
	}

	void BHTree::updateAccel(Body2d & bIn) const
	{
		auto quotient = q.getLength() / (com - bIn.getPos()).mag();
		// if this tree is leaf, use direct calculation
		if ((b != nullptr) && *b != bIn)
		{
			bIn.addAccel(*b);
		}
		// if tree is far enough, use BH approx
		else if (quotient < BH_THETA)
		{
			bIn.addAccel(com, mass);
		}
		// try daughters
		else
		{
			for (auto d : daughters)
			{
				if (d != nullptr)
					d->updateAccel(bIn);
			}
		}
	}

	void BHTree::updateGfx(bool const show_levels)
	{
		// only draw level label if they are switched on and this is a tree leaf
		q.updateGfx((show_levels && n == 1) ? level : -1);
		// recurse
		for (auto d : daughters)
		{
			if (d != nullptr)
				d->updateGfx(show_levels);
		}
	}

	void BHTree::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		// only draw this bounding box if it contains a particle
		if (n > 0)
		{
			target.draw(q);
		}
		// recurse
		for (auto d : daughters)
		{
			if (d != nullptr)
				d->draw(target, states);
		}
	}
}