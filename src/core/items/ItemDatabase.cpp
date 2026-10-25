#include "core/items/ItemDatabase.hpp"

#include <stdexcept>

namespace Oathbound {

    void ItemDatabase::Add(ItemDefinition definition) {
        if (_byId.count(definition.id) || _byAlias.count(definition.id)) {
            throw std::runtime_error("duplicate item id '" + definition.id + "'");
        }
        for (const auto& alias : definition.aliases) {
            if (alias == definition.id || _byId.count(alias) || _byAlias.count(alias)) {
                throw std::runtime_error("item '" + definition.id + "': alias '" + alias + "' is already in use");
            }
        }

        const std::size_t index = _items.size();
        _byId[definition.id] = index;
        for (const auto& alias : definition.aliases) {
            _byAlias[alias] = index;
        }
        _items.push_back(std::move(definition));
    }

    void ItemDatabase::LoadFromJson(const Json& json) {
        auto it = json.find("items");
        if (!json.is_object() || it == json.end() || !it->is_array()) {
            throw std::runtime_error("expected an object with an \"items\" array");
        }
        std::size_t index = 0;
        for (const auto& entry : *it) {
            try {
                Add(ItemDefinition::FromJson(entry));
            } catch (const std::exception& e) {
                throw std::runtime_error("items[" + std::to_string(index) + "]: " + e.what());
            }
            ++index;
        }
    }

    void ItemDatabase::LoadFromFile(const fs::path& path) {
        try {
            LoadFromJson(ReadJsonFile(path));
        } catch (const std::exception& e) {
            throw std::runtime_error(path.string() + ": " + e.what());
        }
    }

    const ItemDefinition* ItemDatabase::Find(const ItemId& id) const {
        if (auto it = _byId.find(id); it != _byId.end()) return &_items[it->second];
        if (auto it = _byAlias.find(id); it != _byAlias.end()) return &_items[it->second];
        return nullptr;
    }

    ItemId ItemDatabase::Canonicalize(const ItemId& id) const {
        if (auto it = _byAlias.find(id); it != _byAlias.end()) return _items[it->second].id;
        return id;
    }
}
