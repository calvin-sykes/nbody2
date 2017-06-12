#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <SFML/Graphics.hpp>

#include <map>
#include <memory>

namespace nbody
{
	class IColourer;
	enum class ColourerType;
	class IIntegrator;
	enum class IntegratorType;
	class IModel;
	enum class ModelType;
	class BodyDistributor;
	enum class DistributorType;

	typedef std::unique_ptr<IColourer>(*ColourerFactory)();
	typedef std::unique_ptr<IIntegrator>(*IntegratorFactory)(IModel *, double);
	typedef std::unique_ptr<IModel>(*ModelFactory)();
	typedef std::unique_ptr<BodyDistributor>(*DistributorFactory)();

	class AssetManager
	{
	public:
		AssetManager() = default;
		
		void loadTexture(std::string const& name, std::string const& filename);
		static void loadFont(std::string const& name, float const size, std::string const& filename);
		void loadIntegrators();
		void loadModels();
		void loadDistributors();
		void loadColourers();

		sf::Texture& getTextureRef(std::string const& name);
		std::unique_ptr<IIntegrator> getIntegrator(IntegratorType const type, IModel * model, double step);
		std::unique_ptr<IModel> getModel(ModelType const type);
		std::unique_ptr<BodyDistributor> getDistributor(DistributorType const type);
		std::unique_ptr<IColourer> getColourer(ColourerType const type);

	private:
		std::map<std::string, sf::Texture> m_textures;
		std::map<std::string, sf::Font> m_fonts;
		std::map<IntegratorType, IntegratorFactory> m_integrators;
		std::map<ColourerType, ColourerFactory> m_colourers;
		std::map<ModelType, ModelFactory> m_models;
		std::map<DistributorType, DistributorFactory> m_distributors;
	};
}

#endif // ASSET_MANAGER_H
