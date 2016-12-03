#include "AssetManager.h"
#include "BodyDistributor.h"
#include "Error.h"
#include "Evolver.h"
#include "Integrator.h"

#include "imgui.h"

#include <fstream>

namespace nbody
{
	void AssetManager::loadTexture(const std::string & name, const std::string & filename)
	{
		sf::Texture tex;
		if (!tex.loadFromFile(filename))
		{
			std::string msg = "Texture " + filename + " not found";
			throw MAKE_ERROR(msg);
		}
		this->textures[name] = tex;
	}

	void AssetManager::loadFont(std::string const & name, float const size, std::string const & filename)
	{
		std::ifstream file;
		file.open(filename, std::ios::binary);
		if (!file.is_open())
			throw MAKE_ERROR(std::string("Could not open file ") + filename);
		file.seekg(0, file.end);
		int len = file.tellg();
		file.seekg(0, file.beg);
		char *buf = new char[len];
		file.read(buf, len);

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromMemoryTTF(buf, len, size, nullptr, io.Fonts->GetGlyphRangesDefault());

		unsigned char* pixels;
		int width;
		int height, bytes_per_pixel;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
	}

	void AssetManager::loadIntegrators()
	{
		this->integrators[IntegratorType::EULER] = std::make_unique<EulerIntegrator>();
	}

	void AssetManager::loadEvolvers()
	{
		this->evolvers[EvolverType::BRUTE_FORCE] = std::make_unique<BruteForceEvolver>();
		this->evolvers[EvolverType::BARNES_HUT] = std::make_unique<BarnesHutEvolver>();
	}

	void AssetManager::loadDistributors()
	{
		this->distributors[DistributorType::EXPONENTIAL] = std::make_unique<ExponentialDistributor>();
		this->distributors[DistributorType::ISOTHERMAL] = std::make_unique<IsothermalDistributor>();
		this->distributors[DistributorType::PLUMMER] = std::make_unique<PlummerDistributor>();
	}

	sf::Texture & AssetManager::getTextureRef(const std::string & name)
	{
		return this->textures.at(name);
	}

	Integrator * AssetManager::getIntegrator(const IntegratorType type)
	{
		return integrators.at(type).get();
	}

	Evolver * AssetManager::getEvolver(const EvolverType type)
	{
		return evolvers.at(type).get();
	}

	BodyDistributor * AssetManager::getDistributor(const DistributorType type)
	{
		return distributors.at(type).get();
	}
}