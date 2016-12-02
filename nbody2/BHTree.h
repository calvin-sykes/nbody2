#ifndef BHTREE_H
#define BHTREE_H

#include "Quad.h"
#include "Vector.h"

#include <SFML/Graphics.hpp>

namespace nbody
{	
	class BHTree;
	class Body2d;

	class BHTree : public sf::Drawable
	{
	public:
		// create empty tree with size given by Quad qIn
		BHTree(Quad const& qIn,  size_t const levelIn);

		// create tree from four daughters
		BHTree(BHTree * nwIn, BHTree * neIn, BHTree * swIn, BHTree * seIn);

		~BHTree();

		// Returns true if segment has no children
		//bool isExternal() const { return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr; };

		void insert(Body2d const* bIn);

		void updateAccel(Body2d &) const;

		void updateGfx(bool const show_levels);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		Vector2d getPCoM() const { return com; }

	private:
		Vector2d com;

		Quad q;

		Body2d const* b;

		size_t n, level;
		size_t static max_level;
		double mass;

		BHTree * daughters[N_DAUGHTERS];
	};
}

#endif // BHTREE_H