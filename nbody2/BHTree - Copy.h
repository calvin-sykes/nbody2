#ifndef BHTREE_H
#define BHTREE_H

#include "Body2d.h"
#include "Quad.h"

#include <memory>
#include <exception>
#include <list>
#include <numeric>
#include <SFML/Graphics.hpp>

namespace nbody
{
	class BHTree;

	typedef std::shared_ptr<BHTree> TreePtr;
	typedef std::shared_ptr<Body2d> BodyPtr;

	class BHTree : public sf::Drawable
	{
	public:
		// create empty tree with size given by Quad qIn
		BHTree(Quad qIn) : q(qIn), b(nullptr), NW(nullptr), NE(nullptr), SW(nullptr), SE(nullptr) {};

		// create tree from four daughters
		BHTree(TreePtr nwIn, TreePtr neIn, TreePtr swIn, TreePtr seIn);

		~BHTree() = default;

		// Returns true if segment has no children
		bool isExternal() const { return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr; };

		void insert(const Body2d& bIn);

		void updateForce(Body2d& bIn) const;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		Vector2d getPCoM() const { return b->getPos(); }
		Vector2d getVCoM() const { return b->getVel(); }

	private:
		BodyPtr b;
		Quad q;
		TreePtr NW;
		TreePtr NE;
		TreePtr SW;
		TreePtr SE;
	};
}

#endif // BHTREE_H