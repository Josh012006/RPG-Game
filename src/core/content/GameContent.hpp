#pragma once

#include <vector>

#include "core/gameplay/ItemUse.hpp"
#include "core/items/ItemDatabase.hpp"
#include "core/world/WorldDatabase.hpp"

namespace Oathbound {

    struct StartingItem {
        ItemId id;
        int quantity = 1;
    };

    // data/new_game.json: what a brand-new character begins with.
    struct NewGameTemplate {
        int level = 1;
        std::size_t inventoryCapacity = 24;
        std::vector<StartingItem> startingItems;
    };

    // Everything that is "game design" rather than "player progress": loaded once at startup,
    // read-only afterwards. Saves refer to it by id.
    struct GameContent {
        ItemDatabase items;
        WorldDatabase worlds;
        NewGameTemplate newGame;
        ItemBehaviorRegistry behaviors;  // code, not data, but it is "what items do"

        // Loads items.json and worlds.json (required) and new_game.json (optional).
        // Throws std::runtime_error with the file name on invalid data.
        static GameContent LoadFromDirectory(const fs::path& dataDir);
    };
}
