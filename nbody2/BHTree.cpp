#include "BHTree.h"
#include "Body2d.h"
#include "Constants.h"
#include <cassert>

namespace nbody
{
	//size_t BHTree::max_level = 0;

	//BHTree::BHTree(const Quad& qIn, const size_t levelIn) :
	//	q(qIn), n(0), level(levelIn), mass(0), com(), b(nullptr), daughters{ nullptr }
	//{
	//	if (level > max_level) max_level = level;
	//}

	//BHTree::BHTree(BHTree * nwIn, BHTree * neIn, BHTree * swIn, BHTree * seIn) :
	//	com(), n(0), mass(0), b(nullptr), daughters{ swIn, seIn, nwIn, neIn }
	//{
	//	// require daughters to be of equal size
	//	assert([&]()->bool {for (size_t i = 0; i < NUM_DAUGHTERS; i++) { if (daughters[0]->q.getLength() != daughters[i]->q.getLength()) return false; } return true; }());

	//	double length = daughters[0]->q.getLength();

	//	// require no gaps between daughters
	//	assert((daughters[static_cast<int>(Daughter::NW)]->q.getXmid() + length / 2) - (daughters[static_cast<int>(Daughter::NE)]->q.getXmid() - length / 2) < 0.0001 * Constants::RADIUS);
	//	assert((daughters[static_cast<int>(Daughter::NW)]->q.getYmid() - length / 2) - (daughters[static_cast<int>(Daughter::SW)]->q.getYmid() + length / 2) < 0.0001 * Constants::RADIUS);

	//	// require daughters to be of same level
	//	assert([&]()->bool {for (size_t i = 0; i < NUM_DAUGHTERS; i++) { if (daughters[static_cast<int>(Daughter::NW)]->level != daughters[i]->level) return false; } return true; }());

	//	level = daughters[0]->level - 1;

	//	// deduce dimensions from a daughter
	//	// choice of SW is arbitrary, any would do
	//	q = Quad(daughters[static_cast<int>(Daughter::SW)]->q.getXmid() + length / 2, daughters[0]->q.getYmid() + length / 2, 2 * length);

	//	// if daughter contains no bodies, prune it
	//	// NB: reference to pointer so that original pointer is set to nullptr
	//	for (auto& d : daughters)
	//	{
	//		if (d->n == 0)
	//		{
	//			delete d;
	//			d = nullptr;
	//		}
	//		else
	//		{
	//			com = (com * mass + d->com * d->mass) * (1.0 / (mass + d->mass));
	//			mass += d->mass;
	//			n += d->n;
	//		}
	//	}
	//}

	//BHTree::~BHTree()
	//{
	//	for (auto d : daughters)
	//	{
	//		if (d != nullptr)
	//			delete d;
	//	}
	//}

	//void BHTree::insert(Body2d const* bIn)
	//{
	//	// which daughter, if any, would contain this body?
	//	auto which_daughter = q.whichDaughter(bIn->getPos());
	//	// If tree doesn't contain body, do nothing
	//	if (which_daughter == Daughter::NONE)
	//	{
	//		return;
	//	}
	//	// If this tree already contains bodies
	//	if (n > 0)
	//	{
	//		// if tree only contains one body it is currently a leaf
	//		// now daughter trees need to be created
	//		if (n == 1)
	//		{
	//			// recursively add current body to correct quadrant
	//			auto current_daughter = q.whichDaughter(b->getPos());
	//			daughters[static_cast<size_t>(current_daughter)] = new BHTree(q.createDaughter(current_daughter), level + 1);
	//			daughters[static_cast<size_t>(current_daughter)]->insert(b);
	//			// tree is no longer leaf
	//			b = nullptr;
	//		}
	//		// create quadrant for new body if it does not exist
	//		if (daughters[static_cast<size_t>(which_daughter)] == nullptr)
	//			daughters[static_cast<size_t>(which_daughter)] = new BHTree(q.createDaughter(which_daughter), level + 1);
	//		// add new body
	//		daughters[static_cast<int>(which_daughter)]->insert(bIn);
	//	}
	//	else // n == 0
	//	{
	//		// store body in this tree leaf
	//		b = bIn;
	//	}
	//	// update centre of mass
	//	com = (com * mass + bIn->getPos() * bIn->getMass()) * (1.0 / (mass + bIn->getMass()));
	//	// increment body counter
	//	n++;
	//	// update mass
	//	mass += bIn->getMass();
	//}

	//void BHTree::updateAccel(Body2d & bIn) const
	//{
	//	auto quotient = q.getLength() / (com - bIn.getPos()).mag();
	//	// if this tree is leaf, use direct calculation
	//	if ((b != nullptr) && *b != bIn)
	//	{
	//		bIn.addAccel(*b);
	//	}
	//	// if tree is far enough, use BH approx
	//	else if (quotient < BH_THETA)
	//	{
	//		bIn.addAccel(com, mass);
	//	}
	//	// try daughters
	//	else
	//	{
	//		for (auto d : daughters)
	//		{
	//			if (d != nullptr)
	//				d->updateAccel(bIn);
	//		}
	//	}
	//}

	/*void BHTree::updateGfx(bool const show_levels)
	{
		// only draw level label if they are switched on and this is a tree leaf
		//q.updateGfx((show_levels && n == 1) ? level : -1);
		// recurse
		for (auto d : daughters)
		{
			if (d != nullptr)
				d->updateGfx(show_levels);
		}
	}*/

	//void BHTree::draw(sf::RenderTarget & target, sf::RenderStates states) const
	//{
	//	// only draw this bounding box if it contains a particle
	//	if (n > 0)
	//	{
	//		//target.draw(q);
	//	}
	//	// recurse
	//	for (auto d : daughters)
	//	{
	//		if (d != nullptr)
	//			d->draw(target, states);
	//	}
	//}
}