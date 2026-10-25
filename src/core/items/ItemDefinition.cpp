#include "core/items/ItemDefinition.hpp"

#include <stdexcept>

namespace Oathbound {

    ItemDefinition ItemDefinition::FromJson(const Json& json) {
        if (!json.is_object()) {
            throw std::runtime_error("item definition must be an object");
        }

        auto text = [&](const char* key, const std::string& fallback) -> std::string {
            auto it = json.find(key);
            if (it == json.end() || it->is_null()) return fallback;
            if (!it->is_string()) throw std::runtime_error(std::string("'") + key + "' must be a string");
            return it->get<std::string>();
        };
        auto integer = [&](const char* key, int fallback) -> int {
            auto it = json.find(key);
            if (it == json.end() || it->is_null()) return fallback;
            if (!it->is_number_integer()) throw std::runtime_error(std::string("'") + key + "' must be an integer");
            return it->get<int>();
        };
        auto stringList = [&](const char* key) -> std::vector<std::string> {
            std::vector<std::string> out;
            auto it = json.find(key);
            if (it == json.end() || it->is_null()) return out;
            if (!it->is_array()) throw std::runtime_error(std::string("'") + key + "' must be an array");
            for (const auto& v : *it) {
                if (!v.is_string()) throw std::runtime_error(std::string("'") + key + "' must only contain strings");
                out.push_back(v.get<std::string>());
            }
            return out;
        };

        ItemDefinition def;
        def.id = text("id", "");
        if (def.id.empty()) throw std::runtime_error("item without an 'id'");

        def.name        = text("name", def.id);
        def.description = text("description", "");
        def.category    = text("category", "misc");
        def.behavior    = text("behavior", "");
        def.maxStack    = integer("maxStack", 1);
        def.value       = integer("value", 0);
        def.aliases     = stringList("aliases");
        def.tags        = stringList("tags");

        if (def.maxStack < 1) {
            throw std::runtime_error("item '" + def.id + "': maxStack must be >= 1");
        }
        if (auto it = json.find("properties"); it != json.end() && !it->is_null()) {
            if (!it->is_object()) throw std::runtime_error("item '" + def.id + "': 'properties' must be an object");
            def.properties = *it;
        }
        if (auto it = json.find("defaultState"); it != json.end() && !it->is_null()) {
            if (!it->is_object()) throw std::runtime_error("item '" + def.id + "': 'defaultState' must be an object");
            def.defaultState = *it;
        }
        return def;
    }
}
