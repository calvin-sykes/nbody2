#include "AssetManager.h"
#include "IColourer.h"
#include "ColourerSolid.h"
#include "ColourerVelocity.h"
#include "IDistributor.h"
#include "DistributorExponential.h"
#include "DistributorIsothermal.h"
#include "DistributorPlummer.h"
#include "Error.h"
#include "IIntegrator.h"
#include "IntegratorEuler.h"
#include "IntegratorEulerImproved.h"
#include "IModel.h"
#include "ModelBruteForce.h"
#include "ModelBarnesHut.h"

#include "imgui.h"

#include <fstream>
#include <memory>

namespace nbody
{
	void AssetManager::loadTexture(const std::string & name, const std::string & filename)
	{
		sf::Texture tex;
		if (!tex.loadFromFile(filename))
		{
			throw MAKE_ERROR("Texture " + filename + " not found");
		}
		m_textures[name] = tex;
	}

	void AssetManager::loadFont(std::string const & name, float const size, std::string const & filename)
	{
		std::ifstream file;
		file.open(filename, std::ios::binary);
		if (!file.is_open())
			throw MAKE_ERROR(std::string("Could not open file ") + filename);
		file.seekg(0, file.end);
		size_t len = file.tellg();
		file.seekg(0, file.beg);
		auto * buf = new char[len];
		file.read(buf, len);

		auto& io = ImGui::GetIO();
		io.Fonts->AddFontFromMemoryTTF(buf, static_cast<int>(len), size, nullptr, io.Fonts->GetGlyphRangesDefault());

		unsigned char* pixels;
		int width;
		int height, bytes_per_pixel;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
	}

	void AssetManager::loadIntegrators()
	{
		m_integrators[IntegratorType::EULER] = IntegratorEuler::create;
		m_integrators[IntegratorType::MODIFIED_EULER] = IntegratorEulerImproved::create;
	}

	void AssetManager::loadModels()
	{
		m_models[ModelType::BRUTE_FORCE] = ModelBruteForce::create;
		m_models[ModelType::BARNES_HUT] = ModelBarnesHut::create;
	}

	void AssetManager::loadDistributors()
	{
		m_distributors[DistributorType::EXPONENTIAL] = DistributorExponential::create;
		m_distributors[DistributorType::ISOTHERMAL] = DistributorIsothermal::create;
		m_distributors[DistributorType::PLUMMER] = DistributorPlummer::create;
	}

	void AssetManager::loadColourers()
	{
		m_colourers[ColourerType::SOLID] = ColourerSolid::create;
		m_colourers[ColourerType::VELOCITY] = ColourerVelocity::create;
	}

	sf::Texture & AssetManager::getTextureRef(const std::string & name)
	{
		return m_textures.at(name);
	}

	std::unique_ptr<IIntegrator> AssetManager::getIntegrator(const IntegratorType type, IModel * model, double step)
	{
		return m_integrators.at(type)(model, step);
	}

	std::unique_ptr<IModel> AssetManager::getModel(ModelType const type)
	{
		return m_models.at(type)();
	}

	std::unique_ptr<IDistributor> AssetManager::getDistributor(const DistributorType type)
	{
		return m_distributors.at(type)();
	}

	std::unique_ptr<IColourer> AssetManager::getColourer(ColourerType const type)
	{
		return m_colourers.at(type)();
	}
}
