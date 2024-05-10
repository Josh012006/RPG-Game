#include "headers/AssetManager.hpp"


namespace Josh {
    void AssetManager::LoadTexture(std::string name, std::string fileName) {
        sf::Texture tex;
        
        // Verifying if the loading process is successful and then adding the texture to our textures map
        if(tex.loadFromFile(fileName)) {
            this->_textures[name] = tex;
        }
    }
    
    sf::Texture& AssetManager::GetTexture(std::string name) {
        // Return the texture with the keyValue name
        return this->_textures.at(name);
    }

    void AssetManager::LoadFont(std::string name, std::string fileName) {
        sf::Font font;

        // Verifying if the loading process is successful and then adding the font to our fonts map
        if(font.loadFromFile(fileName)) {
            this->_fonts[name] = font;
        }
    }

    sf::Font& AssetManager::GetFont(std::string name) {
        // Return the font with the keyValue name
        return this->_fonts.at(name);
    }
}