#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <SFML/Graphics.hpp>

#include <map>
#include <memory>
#include <string>

namespace nbody
{
	class Integrator;
	enum class IntegratorType;
	class Evolver;
	enum class EvolverType;
	class BodyDistributor;
	enum class DistributorType;

	class AssetManager
	{
	public:
		AssetManager() = default;
		
		void loadTexture(std::string const& name, std::string const& filename);
		void loadFont(std::string const& name, float const size, std::string const& filename);
		void loadIntegrators();
		void loadEvolvers();
		void loadDistributors();

		sf::Texture& getTextureRef(std::string const& name);
		Integrator * getIntegrator(IntegratorType const type);
		Evolver * getEvolver(EvolverType const type);
		BodyDistributor * getDistributor(DistributorType const type);

	private:
		std::map<std::string, sf::Texture> textures;
		std::map<std::string, sf::Font> fonts;
		std::map<IntegratorType, std::unique_ptr<Integrator>> integrators;
		std::map<EvolverType, std::unique_ptr<Evolver>> evolvers;
		std::map<DistributorType, std::unique_ptr<BodyDistributor>> distributors;
	};
}

#endif // ASSET_MANAGER_H
