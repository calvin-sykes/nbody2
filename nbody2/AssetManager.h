#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

namespace nbody
{
	class AssetManager
	{
	public:
		AssetManager() = default;
		
		void loadTexture(const std::string& name, const std::string& filename);

		sf::Texture& getTextureRef(const std::string& name);

	private:
		std::map<std::string, sf::Texture> textures;
	};
}

#endif // ASSET_MANAGER_H
