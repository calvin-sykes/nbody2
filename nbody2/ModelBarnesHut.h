#ifndef MODEL_BARNES_HUT_H
#define MODEL_BARNES_HUT_H

#include "BHTreeNode.h"
#include "IModel.h"
#include "Quad.h"

namespace nbody
{
	class ModelBarnesHut : public IModel
	{
	public:
		ModelBarnesHut();
		virtual ~ModelBarnesHut();

		static std::unique_ptr<IModel> create();

		virtual void addBodies(BodyDistributor const& dist, BodyGroupProperties const& bgp);
		virtual void eval(Vector2d * state_in, double time, Vector2d * deriv_out);
		virtual BHTreeNode const* getTreeRoot() const;

	private:
		void calcBounds(ParticleData const& all);
		void buildTree(ParticleData const& all);

		BHTreeNode m_root;
		Quad m_bounds;
	};
}

#endif // !MODEL_BARNES_HUT_H