#pragma once

#include <deque>
#include <unordered_map>

#include "core/items/ItemCatalog.hpp"
#include "core/util/JsonFile.hpp"

namespace Oathbound {

    // All item definitions of the game, loaded from data/items.json.
    class ItemDatabase : public IItemCatalog {
    public:
        // Throws std::runtime_error on duplicate ids or alias conflicts.
        void Add(ItemDefinition definition);

        // Expects {"items": [ {...}, ... ]}. Throws std::runtime_error with the offending index.
        void LoadFromJson(const Json& json);
        void LoadFromFile(const fs::path& path);

        const ItemDefinition* Find(const ItemId& id) const override;
        ItemId Canonicalize(const ItemId& id) const override;

        const std::deque<ItemDefinition>& All() const { return _items; }
        std::size_t Size() const { return _items.size(); }

    private:
        std::deque<ItemDefinition> _items;  // deque: Add() never invalidates earlier pointers
        std::unordered_map<ItemId, std::size_t> _byId;
        std::unordered_map<ItemId, std::size_t> _byAlias;
    };
}
