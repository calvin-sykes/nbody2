#ifndef BHTREE_H
#define BHTREE_H

#include "Body2d.h"
#include "Quad.h"

#include <SFML/Graphics.hpp>

namespace nbody
{	
	class BHTree;
	
	typedef BHTree* pTree; // pointer to BHTree
	typedef Body2d* pBody; // pointer to Body2d
	typedef Body2d const* pcBody; // pointer to const Body2d

	class BHTree : public sf::Drawable
	{
	public:
		// create empty tree with size given by Quad qIn
		BHTree(Quad const& qIn,  size_t const levelIn);

		// create tree from four daughters
		BHTree(pTree nwIn, pTree neIn, pTree swIn, pTree seIn);

		~BHTree();

		// Returns true if segment has no children
		//bool isExternal() const { return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr; };

		void insert(pcBody const bIn);

		void updateAccel(Body2d &) const;

		void updateGfx(const bool show_levels);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		Vector2d getPCoM() const { return com; }

	private:
		Vector2d com;

		Quad q;

		pcBody b;

		size_t n, level;
		static size_t max_level;
		double mass;

		pTree daughters[N_DAUGHTERS];
	};
}

#endif // BHTREE_H