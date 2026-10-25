#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "core/items/ItemCatalog.hpp"
#include "core/items/ItemStack.hpp"

namespace Oathbound {

    // A slot-based inventory.
    //
    // It knows NOTHING about item types. For each stack it only needs:
    //   - the id        (identity)
    //   - maxStack      (asked to the IItemCatalog)
    //   - the opaque `state` (compared and saved, never interpreted)
    // so new weapons, potions, quest items... require zero change here.
    //
    // Invariants enforced on every mutation: quantity in [1, maxStack], only known ids can be
    // added. Loading from a save is deliberately more tolerant (see LoadJson): a player's data is
    // never thrown away just because the game content changed.
    class Inventory {
    public:
        static constexpr std::size_t kMaxCapacity = 4096;

        explicit Inventory(const IItemCatalog& catalog, std::size_t capacity = 24);

        // ---- Queries -------------------------------------------------------------------
        std::size_t Capacity() const { return _slots.size(); }
        std::size_t UsedSlots() const;
        std::size_t FreeSlots() const { return Capacity() - UsedSlots(); }
        bool IsEmpty() const { return UsedSlots() == 0; }

        // nullptr if the slot is empty or out of range.
        const ItemStack* At(std::size_t slot) const;

        // Total quantity of an item (any state), alias-aware.
        int Count(const ItemId& id) const;
        bool Has(const ItemId& id, int quantity = 1) const { return Count(id) >= quantity; }

        // How many units of `stack` could be added right now.
        int RoomFor(const ItemStack& stack) const;

        // Calls fn(slotIndex, const ItemStack&) for every non-empty slot, in slot order.
        template <class Fn>
        void ForEach(Fn&& fn) const {
            for (std::size_t i = 0; i < _slots.size(); ++i) {
                if (_slots[i]) fn(i, *_slots[i]);
            }
        }

        // Incremented on every change: UI refresh and "needs save" checks without observers.
        std::uint64_t Revision() const { return _revision; }

        // ---- Mutations -----------------------------------------------------------------
        // Adds as much as fits (tops up existing stacks first, then fills empty slots).
        // Returns the REMAINDER that did not fit (0 = everything was added).
        // Unknown ids are rejected (whole quantity returned).
        [[nodiscard]] int Add(ItemStack stack);

        // Convenience: uses the definition's defaultState.
        [[nodiscard]] int Add(const ItemId& id, int quantity = 1);

        // All-or-nothing: removes `quantity` units (any state, last slots first) or nothing.
        bool Remove(const ItemId& id, int quantity = 1);

        // Takes up to `quantity` units out of one slot and returns what was taken.
        std::optional<ItemStack> RemoveFromSlot(std::size_t slot, int quantity);

        // Drag & drop: empty target -> move; same kind -> merge (up to maxStack); else swap.
        // Returns false if nothing changed.
        bool Move(std::size_t from, std::size_t to);

        // Merges partial stacks, then orders by category, name, id; packs to the front.
        void Sort();

        void Clear();

        // Grows freely; shrinks only if every slot being removed is empty. Returns success.
        bool SetCapacity(std::size_t capacity);

        // ---- Persistence ---------------------------------------------------------------
        //   {"capacity": 24, "slots": [ {"slot":0,"id":"potion_health","qty":3},
        //                               {"slot":5,"id":"iron_sword","qty":1,"state":{...}} ]}
        // Only occupied slots are written; slot positions are kept so the player's layout survives.
        Json ToJson() const;

        // Replaces the content. NEVER throws and NEVER silently loses a valid item:
        //  - unknown ids are kept as-is (they re-save untouched if the content comes back);
        //  - renamed items are mapped through their aliases;
        //  - colliding / out-of-range slots are relocated, and capacity grows if needed;
        //  - only unusable entries (no id, quantity < 1) are skipped.
        // Every anomaly is reported in `warnings` (if provided) and in the log.
        void LoadJson(const Json& json, std::vector<std::string>* warnings = nullptr);

    private:
        int MaxStackOf(const ItemId& id) const;

        const IItemCatalog* _catalog;
        std::vector<std::optional<ItemStack>> _slots;
        std::uint64_t _revision = 0;
    };
}
