#include "core/content/GameContent.hpp"

#include <algorithm>
#include <stdexcept>

#include "core/util/Log.hpp"

namespace Oathbound {

    GameContent GameContent::LoadFromDirectory(const fs::path& dataDir) {
        GameContent content;
        content.items.LoadFromFile(dataDir / "items.json");
        content.worlds.LoadFromFile(dataDir / "worlds.json");
        RegisterDefaultBehaviors(content.behaviors);

        const fs::path newGamePath = dataDir / "new_game.json";
        if (fs::exists(newGamePath)) {
            try {
                const Json json = ReadJsonFile(newGamePath);
                NewGameTemplate& t = content.newGame;
                if (auto it = json.find("level"); it != json.end() && it->is_number_integer()) {
                    t.level = std::max(1, it->get<int>());
                }
                if (auto it = json.find("inventoryCapacity"); it != json.end() && it->is_number_integer()) {
                    t.inventoryCapacity = static_cast<std::size_t>(std::max(1, it->get<int>()));
                }
                if (auto it = json.find("startingItems"); it != json.end() && it->is_array()) {
                    for (const auto& e : *it) {
                        StartingItem item;
                        item.id = e.value("id", "");
                        item.quantity = e.value("qty", 1);
                        if (!content.items.Find(item.id)) {
                            log::Warn("new_game.json: unknown starting item '" + item.id + "' ignored");
                            continue;
                        }
                        t.startingItems.push_back(std::move(item));
                    }
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(newGamePath.string() + ": " + e.what());
            }
        }

        log::Info("content loaded: " + std::to_string(content.items.Size()) + " items, " +
                  std::to_string(content.worlds.All().size()) + " worlds");
        return content;
    }
}
