#pragma once

#include <map>

#include <SFML/Graphics.hpp>

namespace Josh {
    class AssetManager {
    public:
        AssetManager() {}
        ~AssetManager() {}

        // Function to load a texture with the name "name" and the path "fileName"
        void LoadTexture(std::string name, std::string fileName);
        // A method to get the textures created
        sf::Texture& GetTexture(std::string name);

        // Function to get the fonts
        void LoadFont(std::string name, std::string fileName);
        // A method to get the font created
        sf::Font& GetFont(std::string name);

    private:
        // The maps that is storing our textures and fonts
        std::map<std::string, sf::Texture> _textures;
        std::map<std::string, sf::Font> _fonts;
    };
}
