#ifndef IMODEL_H
#define IMODEL_H

#include "Types.h"
#include "Vector.h"

#include <memory>
#include <vector>

namespace nbody
{
	struct BodyGroupProperties;
	class IDistributor;
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
		constexpr ModelProperties(ModelType const type, char const* name, char const* tooltip)
			: type(type),
			name(name),
			tooltip(tooltip)
		{
		}

		ModelType type;
		char const* name;
		char const* tooltip;
	};

	using ModArray = std::array<ModelProperties, static_cast<size_t>(ModelType::N_MODELS)>;

	constexpr ModArray model_infos = { {
		{
			ModelType::BRUTE_FORCE,
			"Brute-force",
			"Forces between every pair of bodies are calculated directly"
		},
		{
			ModelType::BARNES_HUT,
			"Barnes-Hut",
			"Long-range forces are approximated using a Barnes-Hut tree"
		}
		} };

	class IModel
	{
	public:

		explicit IModel(std::string name, bool has_tree = false, size_t dim = 1);
		virtual ~IModel();

		void init(size_t num_bodies, double step);
		void addBodies(IDistributor const& dist, std::unique_ptr<IColourer> col, BodyGroupProperties const& bgp);
		void updateColours(Vector2d const* all);

		virtual void eval(Vector2d * state, double time, Vector2d * deriv_in) = 0;
		virtual BHTreeNode const* getTreeRoot() const = 0;

		bool hasTree() const;

		size_t getNumBodies() const;
		size_t getNumAdded() const;
		size_t getDim() const;
		void setDim(size_t const dim);

		Vector2d * getInitialStateVector() const;
		ParticleAuxState const* getAuxState() const;
		ParticleColourState const* getColourState() const;
		Vector2d getCentreMass() const;
		std::string const& getName() const;

	protected:
		ParticleState * m_initial_state;
		ParticleAuxState * m_aux_state;
		ParticleColourState * m_colour_state;

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

		std::vector<std::unique_ptr<IColourer>> m_colourers;
	};
}

#endif // !IMODEL_H
