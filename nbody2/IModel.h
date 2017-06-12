#ifndef IMODEL_H
#define IMODEL_H

#include "Types.h"
#include "Vector.h"

namespace nbody
{
	struct BodyGroupProperties;
	class BodyDistributor;
	class BHTreeNode;
	class IColourer;

	enum class ModelType
	{
		BRUTE_FORCE,
		BARNES_HUT,
		N_MODELS,
		INVALID = -1
	};

	struct ModelProperties
	{
		ModelType type;
		char const* name;
		char const* tooltip;
	};

	class IModel
	{
	public:

		explicit IModel(std::string name, bool has_tree = false, size_t dim = 1);
		virtual ~IModel();	
		
		void init(size_t num_bodies, double step);
		void addBodies(BodyDistributor const& dist, BodyGroupProperties const& bgp);
		
		virtual void eval(Vector2d * state, double time, Vector2d * deriv_in) = 0;
		virtual BHTreeNode const* getTreeRoot() const = 0;

		bool hasTree() const;

		size_t getNumBodies() const;
		size_t getNumAdded() const;
		size_t getDim() const;
		void setDim(size_t const dim);

		Vector2d * getInitialState() const;
		ParticleAuxState const* getAuxState() const;
		ParticleColourState* getColourState() const;
		Vector2d getCentreMass() const;
		std::string const& getName() const;

	protected:
		ParticleState * m_initial_state;
		ParticleAuxState * m_aux_state;
		ParticleColourState * m_colours;
		
	protected:
		double m_step;
		size_t m_num_bodies;
		size_t m_num_added;

		double m_tot_mass;
		Vector2d m_centre_mass;

	private:
		void resetDim(size_t num_bodies, double step);

		bool m_has_tree;
		size_t m_dim;
		std::string m_name;
	};
}

#endif // !IMODEL_H
