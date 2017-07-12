#ifndef BH_TREE_NODE_H
#define BH_TREE_NODE_H

#include "Quad.h"
#include "Types.h"
#include "Vector.h"

#include <vector>

namespace nbody
{
	/**
	 * \brief Convenience struct for collecting tree statistics.
	 */
	struct DebugStats
	{
		size_t m_num_calc; // Number of calculations for force estimation
		size_t m_node_ct; // Number of nodes in tree
		size_t m_body_ct; // Number of bodies in tree
		size_t m_max_level; // Deepest level in tree
		size_t m_num_crit_size; // Number of cells containing fewer than CRIT_SIZE bodies
	};

	class BHTreeNode
	{
	public:
		
		/**
		 * \brief Construct a new tree node.
		 * \param q The Quad object encapsulating the physical size of this node.
		 * \param level The dept of this node within the tree. Zero if no value is supplied
		 * \param parent The node from which this node is descended. Null if no value is supplied.
		 */
		explicit BHTreeNode(Quad const& q, size_t const level = 0, BHTreeNode const* parent = nullptr);
		
		
		/**
		 * \brief Recursively destroy this tree node and any daughter nodes.
		 */
		~BHTreeNode();

		/**
		 * \brief Recursively re-initialise this tree node and any daughter nodes.
		 *		  May only be called from the root node.
		 * \param q The Quad object encapsulating the new physical size of the root node.
		 */
		void reset(Quad const& q);

		bool isRoot() const;
		bool isExternal() const;
		bool wasSubdivided() const;

		double getMass() const;
		size_t getLevel() const;
		size_t getNumBodies() const;
		Quad const& getQuad() const;
		Vector2d const& getCentreMass() const;
		
		static size_t getNumRenegades();
		static double getTheta();	
		static DebugStats const& getStats();
		
		/**
		 * \brief Recursively search this tree node and any daughter nodes to determine a point lies within
		 *		  the tree's physical extent.
		 * \param pos The point to test, in world coordinates.
		 * \return If the tree formed by this node and its daughters contains the point, returns a
		 *		   pointer to the deepest node in the tree containing the point. Otherwise, returns null.
		 */
		BHTreeNode const* getHovered(Vector2d const& pos) const;
		
		BHTreeNode const* getParent() const;

		
		/**
		 * \brief Recursively add a new body to the tree formed by this node and its daughters.
		 *		  New daughter nodes will be constructed if required.
		 * \param new_body The body to add to the tree. 
		 */
		void insert(ParticleData const& new_body);
		
		/**
		 * \brief Recursively calculate masses and centres of masses for this cell and its daughters,
		 *		  and dstore a pointer to every node containing fewer than s_CRIT_SIZE bodies in the 
		 *		  vector s_crit_cells. Children of such nodes are not also added to this vector.
		 */
		void computeMassDistribution();

		/**
		 * \brief Recursively assign values to the m_next and m_more pointers used to thread the tree so
		 *		  that it may be traversed iteratively.
		 * \param next Pointer to this node's next sibling, or to the parent node's sibling if this node
		 *			   is the last child. Null if no value is supplied.
		 */
		void threadTree(BHTreeNode * next = nullptr);

		/**
		 * \brief Calculate forces on all bodies within this node.
		 */
		void calcForces() const;

		BHTreeNode *m_daughters[NUM_DAUGHTERS];
		BHTreeNode *m_more, *m_next;

	private:
		/**
		* \brief Recursively reset the m_subdivided flag of this node and any daughter nodes
		*		  used for calculating the m_num_calc statistic.
		*		  May only be called from the root node.
		*/
		void forceCalcStatReset() const;

		/**
		* \brief Reset the m_node_ct, m_body_ct, m_max_level, and m_num_crit_size statistics.
		*		  May only be called from the root node.
		*/
		void treeStatReset() const;
		
		/**
		 * \brief Create a new tree node which will become one of this node's daughters.
		 * \param which The Daughter enumeration specifying which daughter to create.
		 * \return Pointer to the new daughter node.
		 */
		BHTreeNode * createDaughter(Daughter const which) const;
	
		/**
		 * \brief Calculate the acceleration due to the gravitational interaction between two masses.
		 * \param p1 The ParticleData object encapsulating the properties of the first object.
		 * \param p2 The ParticleData object encapsulating the properties of the second object.
		 * \return The acceleration experienced by p1 due to p2. If p1 and p2 have the same coordinates
		 *		   (e.g. they are the same object), a zero vector is returned.
		 */
		static Vector2d calcAccel(ParticleData const& p1, ParticleData const& p2);

		/**
		 * \brief Construct a list of ParticleData objects for which interactions should be evaluated for
		 *		  bodies within this node. The interaction list may contain both external bodies and combined
		 *		  masses from internal nodes for which the BH criterion permits multiple bodies to be
		 *		  aggregated into one.
		 * \param root Pointer to the root node of the tree.
		 * \return The complete list of interactions after the entire tree has been searched.
		 */
		std::vector<ParticleData> makeInteractionList(BHTreeNode const* root) const;
		
		/**
		 * \brief Determine whether the BH criterion permits the bodies within a tree node to be aggregated.
		 * \param node_to_test The tree node to determine the BH criterion for.
		 * \return True if the node satisfies the criterion and the bodies within it may be aggregated.
		 */
		bool accept(BHTreeNode const* node_to_test) const;

		size_t m_level;
		ParticleData m_body;

		// 'combined' particle
		ParticleState m_c_state;
		ParticleAuxState m_c_aux_state;

		double m_rcrit_sq;
		Quad m_quad;
		BHTreeNode const* m_parent;
		size_t m_num;
		mutable bool m_subdivided;

		static std::vector<ParticleData> s_renegades;
		static std::vector<BHTreeNode const*> s_crit_cells;

		double static constexpr s_THETA = 0.9;
		size_t static constexpr s_CRIT_SIZE = 32;
		
		static DebugStats s_stat;
	};
}

#endif // BH_TREE_NODE_H
