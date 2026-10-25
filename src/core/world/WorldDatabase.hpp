#pragma once

#include <optional>
#include <string>
#include <vector>

#include "core/util/JsonFile.hpp"

namespace Oathbound {

    // STATIC description of a world (data/worlds.json). The old struct also carried an sf::Sprite
    // and an sf::FloatRect: rendering state does not belong to game data.
    struct WorldDefinition {
        std::string id;           // stable slug, what saves refer to ("dark_forest")
        std::string name;         // display name
        std::string description;
        std::string difficulty = "Normal";
        std::string background;   // optional image path
        int minimumLevel = 1;

        // "locked" used to be hard-coded in the JSON; it is now derived from the player.
        bool IsUnlockedFor(int playerLevel) const { return playerLevel >= minimumLevel; }
    };

    class WorldDatabase {
    public:
        // Expects {"worlds": [ {...}, ... ]}. Throws std::runtime_error on invalid data.
        void LoadFromJson(const Json& json);
        void LoadFromFile(const fs::path& path);

        const std::vector<WorldDefinition>& All() const { return _worlds; }
        const WorldDefinition* Find(const std::string& id) const;
        const WorldDefinition* FindByName(const std::string& name) const;  // used by legacy-save migration
        std::optional<std::size_t> IndexOf(const std::string& id) const;

    private:
        std::vector<WorldDefinition> _worlds;
    };
}
