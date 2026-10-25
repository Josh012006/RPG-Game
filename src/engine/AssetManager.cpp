#include "engine/AssetManager.hpp"

#include <stdexcept>

namespace Oathbound {

    const sf::Texture& AssetManager::Texture(const std::string& path) {
        auto it = _textures.find(path);
        if (it != _textures.end()) return *it->second;

        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(path)) {
            throw std::runtime_error("cannot load texture '" + path + "'");
        }
        return *(_textures[path] = std::move(texture));
    }

    const sf::Font& AssetManager::Font(const std::string& path) {
        auto it = _fonts.find(path);
        if (it != _fonts.end()) return *it->second;

        auto font = std::make_unique<sf::Font>();
        if (!font->loadFromFile(path)) {
            throw std::runtime_error("cannot load font '" + path + "'");
        }
        return *(_fonts[path] = std::move(font));
    }
}
