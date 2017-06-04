#ifndef IMODEL_H
#define IMODEL_H

#include "Types.h"
#include "Vector.h"

#include <memory>
#include <string>

namespace nbody
{
	struct BodyGroupProperties;
	class BodyDistributor;

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

		IModel(std::string name, size_t dim = 1);
		virtual ~IModel();	
		
		void init(size_t num_bodies, double step);

		virtual void addBodies(BodyDistributor const& dist, BodyGroupProperties const& bgp) = 0;
		virtual void eval(Vector2d * state, double time, Vector2d * deriv_in) = 0;

		size_t getNumBodies() const;
		size_t getDim() const;
		void setDim(size_t const dim);

		Vector2d * getInitialState();
		ParticleAuxState const* getAuxState() const;
		Vector2d getCentreMass() const;
		std::string const& getName() const;

	protected:
		ParticleState * m_initial_state;
		ParticleAuxState * m_aux_state;

		double m_step;
		size_t m_num_bodies;
		size_t m_num_added;

		double m_tot_mass;
		Vector2d m_centre_mass;

	private:

		void resetDim(size_t num_bodies, double step);

		size_t m_dim;
		std::string m_name;
	};
}

#endif // !IMODEL_H