#include "core/save/SaveCodec.hpp"

#include <algorithm>

#include "core/save/SaveError.hpp"
#include "core/util/Log.hpp"

namespace Oathbound {

    namespace {
        // Reads obj[key] as T, or `fallback` if absent / wrong type.
        template <class T>
        T GetOr(const Json& obj, const char* key, const T& fallback) {
            if (!obj.is_object()) return fallback;
            auto it = obj.find(key);
            if (it == obj.end() || it->is_null()) return fallback;
            try {
                return it->get<T>();
            } catch (const Json::exception&) {
                return fallback;
            }
        }
    }

    Json EncodeSession(const GameSession& session) {
        Json meta = Json::object();
        meta["createdAt"] = session.createdAt;
        meta["updatedAt"] = session.updatedAt;
        meta["playTimeSeconds"] = session.playTimeSeconds;

        const PlayerProfile& p = session.player;
        Json player = Json::object();
        player["name"] = p.name;
        player["level"] = p.level;
        player["health"] = p.health;
        player["maxHealth"] = p.maxHealth;
        player["currentWorldId"] = p.currentWorldId;
        player["inventory"] = p.inventory.ToJson();

        Json root = Json::object();
        root["version"] = kCurrentSaveVersion;
        root["meta"] = std::move(meta);
        root["player"] = std::move(player);
        return root;
    }

    Json MigrateSaveData(Json data, const WorldDatabase& worlds) {
        if (!data.is_object()) throw SaveError("save root is not a JSON object");

        const int version = GetOr<int>(data, "version", 1);  // v1 files had no version field
        if (version > kCurrentSaveVersion) {
            throw SaveTooNewError("save has version " + std::to_string(version) +
                                  " but this game only understands up to " + std::to_string(kCurrentSaveVersion));
        }
        if (version < 1) throw SaveError("invalid save version " + std::to_string(version));

        int current = version;

        if (current == 1) {
            auto playerIt = data.find("player");
            if (playerIt == data.end() || !playerIt->is_object()) throw SaveError("save has no 'player' object");
            Json& player = *playerIt;

            // The world used to be stored by display name; it is now a stable id.
            if (auto it = player.find("currentWorld"); it != player.end()) {
                if (it->is_string()) {
                    if (const WorldDefinition* w = worlds.FindByName(it->get<std::string>())) {
                        player["currentWorldId"] = w->id;
                    } else {
                        log::Warn("migration: unknown world '" + it->get<std::string>() + "', left unset");
                    }
                }
                player.erase("currentWorld");
            }
            data["version"] = 2;
            current = 2;
        }

        // if (current == 2) { ...migrate to 3...; current = 3; }

        return data;
    }

    GameSession DecodeSession(const Json& data, const GameContent& content, std::vector<std::string>* warnings) {
        auto playerIt = data.find("player");
        if (!data.is_object() || playerIt == data.end() || !playerIt->is_object()) {
            throw SaveError("save has no 'player' object");
        }
        const Json& p = *playerIt;

        const std::string name = GetOr<std::string>(p, "name", "");
        if (name.empty()) throw SaveError("save has no player name");

        PlayerProfile profile(content.items, name, content.newGame.inventoryCapacity);
        profile.level = std::max(1, GetOr<int>(p, "level", 1));
        profile.maxHealth = std::max(1, GetOr<int>(p, "maxHealth", 100));
        profile.health = std::clamp(GetOr<int>(p, "health", profile.maxHealth), 0, profile.maxHealth);
        profile.currentWorldId = GetOr<std::string>(p, "currentWorldId", "");

        if (auto invIt = p.find("inventory"); invIt != p.end()) {
            profile.inventory.LoadJson(*invIt, warnings);
        }

        GameSession session(std::move(profile));
        if (auto metaIt = data.find("meta"); metaIt != data.end()) {
            session.createdAt = GetOr<std::string>(*metaIt, "createdAt", "");
            session.updatedAt = GetOr<std::string>(*metaIt, "updatedAt", "");
            session.playTimeSeconds = std::max(0.0, GetOr<double>(*metaIt, "playTimeSeconds", 0.0));
        }
        return session;
    }
}
