#pragma once

#include <string>
#include <vector>

#include "core/util/Json.hpp"

namespace Oathbound {

    // Items are identified by a stable string id ("potion_health"), never by an enum or an index:
    // adding, removing or reordering items can never corrupt an existing save.
    using ItemId = std::string;

    // The STATIC description of an item kind, loaded from data/items.json.
    //
    // The inventory only ever needs `id` and `maxStack`. Everything specific to a kind of item
    // (damage, heal amount, element...) lives in the opaque `properties` bag and is read only by
    // the system that cares about it (combat reads "damage", a consumable reads "heal"...).
    // That is what lets you add item types without touching the inventory code.
    struct ItemDefinition {
        ItemId id;
        std::string name;
        std::string description;
        std::string category = "misc";   // free text: "weapon", "consumable", "quest"...
        std::string behavior;            // key into ItemBehaviorRegistry; empty = use `category`
        int maxStack = 1;
        int value = 0;
        std::vector<ItemId> aliases;     // former ids of this item (renames keep old saves valid)
        std::vector<std::string> tags;
        Json properties = Json::object();  // opaque, type-specific static data
        Json defaultState;                 // initial per-instance state (null = none), e.g. {"durability":100}

        // Reads a typed property, returning `fallback` if it is missing or has the wrong type.
        template <class T>
        T Property(const std::string& key, const T& fallback) const {
            if (!properties.is_object()) return fallback;
            auto it = properties.find(key);
            if (it == properties.end()) return fallback;
            try {
                return it->template get<T>();
            } catch (const Json::exception&) {
                return fallback;
            }
        }

        // Throws std::runtime_error with a readable message if the JSON is not a valid item.
        static ItemDefinition FromJson(const Json& json);
    };
}
