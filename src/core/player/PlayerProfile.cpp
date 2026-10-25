#include "core/player/PlayerProfile.hpp"

#include <algorithm>

namespace Oathbound::player_name {

    std::string Normalize(const std::string& raw) {
        const auto first = raw.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        const auto last = raw.find_last_not_of(' ');
        return raw.substr(first, last - first + 1);
    }

    bool IsValid(const std::string& name) {
        if (name.empty() || name.size() > kMaxLength) return false;
        return std::all_of(name.begin(), name.end(),
                           [](unsigned char c) { return c >= 32 && c < 127; });
    }
}
