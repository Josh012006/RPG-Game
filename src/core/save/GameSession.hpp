#pragma once

#include <string>

#include "core/player/PlayerProfile.hpp"

namespace Oathbound {

    // Everything that belongs to one running game and one save file. This replaces the old
    // approach where a loaded save was parsed and then thrown away: states now share ONE
    // GameSession instead of each re-reading JSON files on their own.
    struct GameSession {
        std::string saveId;        // file name without extension ("save_1")
        std::string createdAt;     // ISO-8601 UTC
        std::string updatedAt;
        double playTimeSeconds = 0.0;
        PlayerProfile player;

        explicit GameSession(PlayerProfile profile) : player(std::move(profile)) {}
    };
}
