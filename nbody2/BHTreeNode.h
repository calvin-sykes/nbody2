#ifndef BH_TREE_NODE_H
#define BH_TREE_NODE_H

#include "Quad.h"
#include "Types.h"
#include "Vector.h"

#include <vector>

namespace nbody
{
	class BHTreeNode
	{
	public:
		
		struct DebugStat
		{
			size_t m_num_calc; // Number of calculations for force estimation
		};
		
		BHTreeNode(Quad const& q, BHTreeNode const* parent = nullptr);
		~BHTreeNode();

		void reset(Quad const& q);

		bool isRoot() const;
		bool isExternal() const;

		bool wasTooClose() const;

		size_t getNumBodies() const;
		size_t getNumRenegades() const;

		Quad const& getQuad() const;

		double getTheta() const;
		void setTheta(double theta);

		size_t getStatNumCalc() const;
		void statReset() const;

		void computeMassDistribution();
		const Vector2d & getCentreMass() const;

		void insert(ParticleData const& new_body, size_t level);

		Vector2d calcForce(ParticleData const& p);

		BHTreeNode * m_daughters[NUM_DAUGHTERS];

	private:
		BHTreeNode * createDaughter(Quad const& q) const;
		
		Vector2d calcAccel(ParticleData const& p1, ParticleData const& p2);
		Vector2d calcTreeForce(ParticleData const& p);

		ParticleData m_body;
		double m_mass;
		Vector2d m_centre_mass;
		Quad m_quad;
		BHTreeNode const* m_parent;
		size_t m_num;
		mutable bool m_subdivided;

		static std::vector<ParticleData> s_renegades;
		static BHTreeNode::DebugStat s_stat;
		static double s_bh_theta;
	};
}

#endif // BH_TREE_NODE_H
