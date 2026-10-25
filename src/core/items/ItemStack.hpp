#pragma once

#include <utility>

#include "core/items/ItemDefinition.hpp"

namespace Oathbound {

    inline bool IsEmptyState(const Json& state) {
        return state.is_null() || (state.is_object() && state.empty());
    }

    // An INSTANCE of an item in the player's hands: which kind, how many, and an optional
    // per-instance `state` (durability, enchantments, a custom name...).
    //
    // `state` is deliberately an opaque JSON value: the inventory stores it, compares it and
    // saves it without ever interpreting it. Two stacks only merge if id AND state are equal,
    // so a worn sword and a new sword never melt into one stack.
    struct ItemStack {
        ItemId id;
        int quantity = 1;
        Json state;  // null = no per-instance data

        ItemStack() = default;
        ItemStack(ItemId itemId, int count = 1, Json itemState = Json())
            : id(std::move(itemId)), quantity(count), state(std::move(itemState)) {}

        bool SameKindAs(const ItemStack& other) const {
            return id == other.id && (state == other.state || (IsEmptyState(state) && IsEmptyState(other.state)));
        }

        friend bool operator==(const ItemStack& a, const ItemStack& b) {
            return a.quantity == b.quantity && a.SameKindAs(b);
        }
        friend bool operator!=(const ItemStack& a, const ItemStack& b) { return !(a == b); }
    };
}
