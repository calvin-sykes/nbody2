#ifndef BH_TREE_NODE_H
#define BH_TREE_NODE_H

#include "Quad.h"
#include "Types.h"
#include "Vector.h"

#include <forward_list>
#include <vector>

namespace nbody
{
	struct DebugStats
	{
		size_t m_num_calc; // Number of calculations for force estimation
		size_t m_node_ct; // Number of nodes in tree
		size_t m_body_ct; // Number of bodies in tree
		size_t m_max_level; // Deepest level in tree
		size_t m_num_crit_size; // Number of cells containing fewer than CRIT_SIZE bodies
	};

	class BHTreeNode;

	class BHTreeNode
	{
	public:
		explicit BHTreeNode(Quad const& q, size_t const level = 0, BHTreeNode const* parent = nullptr);
		~BHTreeNode();

		void reset(Quad const& q);

		bool isRoot() const;
		bool isExternal() const;

		bool wasSubdivided() const;

		size_t getNumBodies() const;
		static size_t getNumRenegades();

		Quad const& getQuad() const;

		static double getTheta();
		size_t getLevel() const;

		static DebugStats const& getStats();
		void forceCalcStatReset() const;
		void treeStatReset() const;

		void computeMassDistribution();
		void computeCritSizeCells() const;
		Vector2d const& getCentreMass() const;
		double getMass() const;

		BHTreeNode const* getHovered(Vector2d const& pos) const;
		BHTreeNode const* getParent() const;

		void insert(ParticleData const& new_body, size_t level);
		void threadTree(BHTreeNode * next = nullptr);

		void calcForces() const;

		BHTreeNode *m_daughters[NUM_DAUGHTERS];
		BHTreeNode *m_more, *m_next;

	private:
		BHTreeNode * createDaughter(Quad const& q) const;

		static Vector2d calcAccel(ParticleData const& p1, ParticleData const& p2);

		static std::forward_list<ParticleData> makeInteractionList(BHTreeNode const* root, BHTreeNode const* group);
		static bool accept(BHTreeNode const* n, BHTreeNode const* group);


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
