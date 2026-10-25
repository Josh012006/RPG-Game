#pragma once

#include <nlohmann/json.hpp>

namespace Oathbound {
    // One alias for the whole project, so the JSON library can be swapped in one place.
    using Json = nlohmann::json;
}
