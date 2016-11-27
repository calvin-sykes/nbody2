#include "AssetManager.h"
#include "Error.h"

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

	sf::Texture & AssetManager::getTextureRef(const std::string & name)
	{
		return this->textures.at(name);
	}
}