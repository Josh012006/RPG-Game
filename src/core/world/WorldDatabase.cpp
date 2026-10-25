#include "core/world/WorldDatabase.hpp"

#include <algorithm>
#include <stdexcept>

namespace Oathbound {

    void WorldDatabase::LoadFromJson(const Json& json) {
        auto it = json.find("worlds");
        if (!json.is_object() || it == json.end() || !it->is_array()) {
            throw std::runtime_error("expected an object with a \"worlds\" array");
        }

        std::vector<WorldDefinition> loaded;
        for (std::size_t i = 0; i < it->size(); ++i) {
            const Json& e = (*it)[i];
            auto text = [&](const char* key, const std::string& fallback) {
                auto f = e.find(key);
                return (f != e.end() && f->is_string()) ? f->get<std::string>() : fallback;
            };

            WorldDefinition w;
            w.id = text("id", "");
            w.name = text("name", "");
            if (w.id.empty() || w.name.empty()) {
                throw std::runtime_error("worlds[" + std::to_string(i) + "]: 'id' and 'name' are required strings");
            }
            if (std::any_of(loaded.begin(), loaded.end(), [&](const auto& o) { return o.id == w.id; })) {
                throw std::runtime_error("duplicate world id '" + w.id + "'");
            }
            w.description = text("description", "");
            w.difficulty = text("difficulty", "Normal");
            w.background = text("background", "");
            if (auto f = e.find("minimumLevel"); f != e.end() && f->is_number_integer()) {
                w.minimumLevel = std::max(1, f->get<int>());
            }
            loaded.push_back(std::move(w));
        }
        _worlds = std::move(loaded);
    }

    void WorldDatabase::LoadFromFile(const fs::path& path) {
        try {
            LoadFromJson(ReadJsonFile(path));
        } catch (const std::exception& e) {
            throw std::runtime_error(path.string() + ": " + e.what());
        }
    }

    const WorldDefinition* WorldDatabase::Find(const std::string& id) const {
        auto it = std::find_if(_worlds.begin(), _worlds.end(), [&](const auto& w) { return w.id == id; });
        return it == _worlds.end() ? nullptr : &*it;
    }

    const WorldDefinition* WorldDatabase::FindByName(const std::string& name) const {
        auto it = std::find_if(_worlds.begin(), _worlds.end(), [&](const auto& w) { return w.name == name; });
        return it == _worlds.end() ? nullptr : &*it;
    }

    std::optional<std::size_t> WorldDatabase::IndexOf(const std::string& id) const {
        for (std::size_t i = 0; i < _worlds.size(); ++i) {
            if (_worlds[i].id == id) return i;
        }
        return std::nullopt;
    }
}
