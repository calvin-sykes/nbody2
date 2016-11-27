#include "BHTree.h"
#include "Constants.h"

namespace nbody
{
	BHTree::BHTree(TreePtr nwIn, TreePtr neIn, TreePtr swIn, TreePtr seIn)
	{
		NW = std::move(nwIn);
		NE = std::move(neIn);
		SW = std::move(swIn);
		SE = std::move(seIn);
		// deduce dimensions from daughters
		double length;
		try
		{
			// require daughters to be of equal size
			if (!(NW->q.getLength() == NE->q.getLength()
				&& NW->q.getLength() == SW->q.getLength()
				&& NW->q.getLength() == SE->q.getLength()))
				throw std::invalid_argument("Constructing BHTree: daughter quads have different lengths");
			else
				length = NW->q.getLength();
			// require no gaps between daughters
			if (!((NW->q.getXmid() + length / 2) - (NE->q.getXmid() - length / 2) < 0.0001 * RAD) &&
				!((NW->q.getYmid() - length / 2) - (SW->q.getYmid() + length / 2) < 0.0001 * RAD))
			{
				throw std::invalid_argument("Constructing BHTree: daughters not space-filling");
			}
		}
		catch (std::invalid_argument except)
		{
			throw except;
		}

		q = std::move(Quad(NW->q.getXmid() + length / 2., NW->q.getYmid() - length / 2., 2 * length, true));

		b = std::make_shared<Body2d>();

		// find subtrees with non-null bodies and merge them in
		// if body is null, prune the subtree
		if (NW->b == nullptr)
			NW.reset();
		else
			b = std::make_shared<Body2d>(std::move(Body2d::combine(*b, *(NW->b))));
		if (NE->b == nullptr)
			NE.reset();
		else
			b = std::make_shared<Body2d>(std::move(Body2d::combine(*b, *(NE->b))));
		if (SW->b == nullptr)
			SW.reset();
		else
			b = std::make_shared<Body2d>(std::move(Body2d::combine(*b, *(SW->b))));
		if (SE->b == nullptr)
			SE.reset();
		else
			b = std::make_shared<Body2d>(std::move(Body2d::combine(*b, *(SE->b))));
	};

	void BHTree::insert(const Body2d& bIn)
	{
		// If there is no body in this tree segment, place it here.
		if (b == nullptr)
			b = std::make_shared<Body2d>(bIn);

		// If segment is an internal segment, recursively insert body to appropriate quadrant
		// update the 'centre-of-mass' body contained in this node
		else if (!isExternal())
		{
			// combine bodies
			b = std::make_shared<Body2d>(std::move(Body2d::combine(*b, bIn)));

			Quad northwest = q.NW();
			if (bIn.in(northwest))
			{
				if (NW == nullptr) NW = std::make_unique<BHTree>(std::move(northwest));
				NW->insert(bIn);
			}
			else
			{
				Quad northeast = q.NE();
				if (bIn.in(northeast))
				{
					if (NE == nullptr) NE = std::make_unique<BHTree>(std::move(northeast));
					NE->insert(bIn);
				}
				else
				{
					Quad southwest = q.SW();
					if (bIn.in(southwest))
					{
						if (SW == nullptr) SW = std::make_unique<BHTree>(std::move(southwest));
						SW->insert(bIn);
					}
					else
					{
						Quad southeast = q.SE();
						if (SE == nullptr) SE = std::make_unique<BHTree>(std::move(southeast));
						SE->insert(bIn);
					}
				}
			}
		}
		// node is external and contains another body
		else // isExternal() == true
		{
			Quad northwest = q.NW();
			if (b->in(northwest))
			{
				if (NW == nullptr) NW = std::make_unique<BHTree>(std::move(northwest));
				NW->insert(*b);
			}
			else
			{
				Quad northeast = q.NE();
				if (b->in(northeast))
				{
					if (NE == nullptr) NE = std::make_unique<BHTree>(std::move(northeast));
					NE->insert(*b);
				}
				else
				{
					Quad southwest = q.SW();
					if (b->in(southwest))
					{
						if (SW == nullptr) SW = std::make_unique<BHTree>(std::move(southwest));
						SW->insert(*b);
					}
					else
					{
						Quad southeast = q.SE();
						if (SE == nullptr) SE = std::make_unique<BHTree>(std::move(southeast));
						SE->insert(*b);
					}
				}
			}
			insert(bIn);
		}
	}

	void BHTree::updateForce(Body2d& bIn) const
	{
		auto quotient = q.getLength() / b->distanceTo(bIn);
		if (isExternal())
		{
			if (*b != bIn) bIn.addForce(*b);
		}
		else if (quotient < BH_THETA)
		{
			bIn.addForce(*b);
		}
		else
		{
			if (NW != nullptr) NW->updateForce(bIn);
			if (SW != nullptr) SW->updateForce(bIn);
			if (SE != nullptr) SE->updateForce(bIn);
			if (NE != nullptr) NE->updateForce(bIn);
		}
	}

	void BHTree::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		// draw this bounding box
		target.draw(q);
		// recurse to deeper nodes
		if (NW != nullptr) NW->draw(target, states);
		if (SW != nullptr) SW->draw(target, states);
		if (SE != nullptr) SE->draw(target, states);
		if (NE != nullptr) NE->draw(target, states);
	}
}