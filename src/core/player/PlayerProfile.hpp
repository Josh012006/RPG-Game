#pragma once

#include <string>

#include "core/inventory/Inventory.hpp"

namespace Oathbound {

    // Pure player DATA (no sprite, no window): this is what gets saved and loaded.
    // Rendering lives elsewhere (engine/PlayerAvatar).
    struct PlayerProfile {
        std::string name;
        int level = 1;
        int health = 100;
        int maxHealth = 100;
        std::string currentWorldId;  // id from worlds.json ("" = none chosen yet)
        Inventory inventory;

        PlayerProfile(const IItemCatalog& catalog, std::string playerName, std::size_t inventoryCapacity = 24)
            : name(std::move(playerName)), inventory(catalog, inventoryCapacity) {}
    };

    namespace player_name {
        inline constexpr std::size_t kMaxLength = 20;

        // Trims leading/trailing spaces.
        std::string Normalize(const std::string& raw);

        // 1..kMaxLength printable ASCII characters (the game font has no accents for now).
        bool IsValid(const std::string& name);
    }
}
