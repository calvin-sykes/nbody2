#ifndef BH_TREE_NODE_H
#define BH_TREE_NODE_H

#include "Quad.h"
#include "Types.h"
#include "Vector.h"

#include <vector>

namespace nbody
{
	struct DebugStats
	{
		size_t m_num_calc; // Number of calculations for force estimation
		size_t m_node_ct; // Number of nodes in tree
		size_t m_body_ct; // Number of bodies in tree
		size_t m_max_level; // Deepest level in tree
	};

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
		static void setTheta(double theta);
		size_t getLevel() const;

		static DebugStats const& getStats();
		void forceCalcStatReset() const;
		void treeStatReset() const;

		void computeMassDistribution();
		Vector2d const& getCentreMass() const;

		BHTreeNode const* getHovered(Vector2d const& pos) const;
		BHTreeNode const* getParent() const;

		void insert(ParticleData const& new_body, size_t level);

		Vector2d calcForce(ParticleData const& p);

		BHTreeNode * m_daughters[NUM_DAUGHTERS];

	private:
		BHTreeNode * createDaughter(Quad const& q) const;
		
		Vector2d calcAccel(ParticleData const& p1, ParticleData const& p2) const;
		Vector2d calcTreeForce(ParticleData const& p);

		size_t m_level;
		ParticleData m_body;
		double m_mass;
		double m_rcrit_sq;
		Vector2d m_centre_mass;
		Quad m_quad;
		BHTreeNode const* m_parent;
		size_t m_num;
		mutable bool m_subdivided;

		static std::vector<ParticleData> s_renegades;
		static DebugStats s_stat;
		static double s_bh_theta;
	};
}

#endif // BH_TREE_NODE_H
