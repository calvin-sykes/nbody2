#ifndef MODEL_BARNES_HUT_H
#define MODEL_BARNES_HUT_H

#include "BHTreeNode.h"
#include "IModel.h"
#include "Quad.h"

namespace nbody
{	
	// From http://www.richelbilderbeek.nl/CppAccumulate_if.htm
	namespace priv
	{
		template<typename InputIterator, typename ElementType, typename BinaryOperation, typename Predicate>
		ElementType accumulate_if(
			InputIterator first,
			const InputIterator last,
			ElementType init,
			const BinaryOperation binary_op,
			const Predicate predicate)
		{
			for (; first != last; ++first)
				if (predicate(*first)) init = binary_op(init, *first);
			return init;
		}
	}

	class ModelBarnesHut : public IModel
	{
	public:
		ModelBarnesHut();
		virtual ~ModelBarnesHut();

		static std::unique_ptr<IModel> create();

		//void addBodies(IDistributor const& m_dist, BodyGroupProperties const& bgp) override;
		void eval(Vector2d * state_in, double time, Vector2d * deriv_out) override;
		BHTreeNode const* getTreeRoot() const override;

	private:
		void calcBounds(ParticleData const& all);
		void buildTree(ParticleData const& all);

		BHTreeNode m_root;
		Quad m_bounds;
	};
}

#endif // !MODEL_BARNES_HUT_H