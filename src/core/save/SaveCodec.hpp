#pragma once

#include <string>
#include <vector>

#include "core/content/GameContent.hpp"
#include "core/save/GameSession.hpp"

namespace Oathbound {

    // Bump this when the save layout changes, and add a step in MigrateSaveData().
    //   v1: {"player": {"name","level","currentWorld"(display name)}}      (original prototype)
    //   v2: adds "version", "meta", health, "currentWorldId" and "player.inventory"
    inline constexpr int kCurrentSaveVersion = 2;

    // GameSession -> JSON (always the current version).
    Json EncodeSession(const GameSession& session);

    // Upgrades any older layout to the current one, step by step (v1 -> v2 -> ...).
    // Throws SaveError for unreadable data and SaveTooNewError for a future version.
    Json MigrateSaveData(Json data, const WorldDatabase& worlds);

    // JSON (already migrated) -> GameSession. Lenient on purpose: missing optional fields get
    // defaults, and the inventory is loaded with Inventory::LoadJson (never loses items).
    // Throws SaveError only if the save is structurally unusable (no "player" / no name).
    GameSession DecodeSession(const Json& migrated, const GameContent& content,
                              std::vector<std::string>* warnings = nullptr);
}
