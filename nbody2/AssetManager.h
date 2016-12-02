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
		
		void loadTexture(const std::string& name, const std::string& filename);
		void loadIntegrators();
		void loadEvolvers();
		void loadDistributors();

		sf::Texture& getTextureRef(const std::string& name);
		Integrator * getIntegrator(const IntegratorType type);
		Evolver * getEvolver(const EvolverType type);
		BodyDistributor * getDistributor(const DistributorType type);

	private:
		std::map<std::string, sf::Texture> textures;
		std::map<IntegratorType, std::unique_ptr<Integrator>> integrators;
		std::map<EvolverType, std::unique_ptr<Evolver>> evolvers;
		std::map<DistributorType, std::unique_ptr<BodyDistributor>> distributors;
	};
}

#endif // ASSET_MANAGER_H
