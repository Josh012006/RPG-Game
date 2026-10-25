#pragma once

#include <ctime>
#include <string>

namespace Oathbound {
    // Current UTC time as "2026-06-24T22:57:00Z". ISO-8601 strings sort chronologically.
    inline std::string NowIso8601Utc() {
        std::time_t now = std::time(nullptr);
        std::tm utc{};
#ifdef _WIN32
        gmtime_s(&utc, &now);
#else
        gmtime_r(&now, &utc);
#endif
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc);
        return buffer;
    }
}
