#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace Oathbound {

    // Loads each file once and keeps it alive for the whole game. Resources are keyed by their
    // file path, so there is no second naming scheme to keep in sync ("MainMenu" vs the path).
    //
    // Returned references stay valid until the AssetManager is destroyed (resources are heap
    // allocated), so sprites and texts can safely keep pointing at them. The old CharacterManager
    // kept a private COPY of the whole AssetManager, which left its sprite pointing into a copy.
    //
    // A missing file throws std::runtime_error naming the path (the old code ignored the failure
    // and crashed later with an unhelpful std::out_of_range).
    class AssetManager {
    public:
        const sf::Texture& Texture(const std::string& path);
        const sf::Font& Font(const std::string& path);

    private:
        std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;
        std::unordered_map<std::string, std::unique_ptr<sf::Font>> _fonts;
    };
}
