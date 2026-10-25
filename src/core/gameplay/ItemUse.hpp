#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "core/items/ItemCatalog.hpp"
#include "core/items/ItemStack.hpp"
#include "core/player/PlayerProfile.hpp"

namespace Oathbound {

    enum class UseOutcome {
        NotUsable,  // this kind of item has no "use" action
        Failed,     // usable, but not right now (e.g. already at full health); nothing consumed
        Kept,       // used, item stays (a key, a tool...)
        Consumed    // used, one unit is removed from the stack
    };

    struct UseResult {
        UseOutcome outcome = UseOutcome::NotUsable;
        std::string message;
    };

    // Extension point: ONE class per kind of behaviour, registered under a key.
    // The inventory never sees any of this; adding a behaviour touches no existing code.
    class IItemBehavior {
    public:
        virtual ~IItemBehavior() = default;
        virtual UseResult Use(PlayerProfile& player, const ItemDefinition& definition, const ItemStack& stack) const = 0;
    };

    class ItemBehaviorRegistry {
    public:
        void Register(const std::string& key, std::shared_ptr<IItemBehavior> behavior);
        const IItemBehavior* Find(const std::string& key) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<IItemBehavior>> _behaviors;
    };

    // Registers the built-in behaviours ("consumable").
    void RegisterDefaultBehaviors(ItemBehaviorRegistry& registry);

    // Uses the item in `slot`: finds its behaviour (definition.behavior, else its category),
    // runs it, and removes one unit if the result is Consumed.
    UseResult UseItem(PlayerProfile& player, std::size_t slot, const IItemCatalog& catalog,
                      const ItemBehaviorRegistry& registry);
}
