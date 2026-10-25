#pragma once

#include <iostream>
#include <string>

namespace Oathbound::log {
    // Minimal logging. Replace the bodies with spdlog & co. later without touching callers.
    inline void Info(const std::string& message)  { std::cout << "[info]  " << message << '\n'; }
    inline void Warn(const std::string& message)  { std::cerr << "[warn]  " << message << '\n'; }
    inline void Error(const std::string& message) { std::cerr << "[error] " << message << '\n'; }
}
