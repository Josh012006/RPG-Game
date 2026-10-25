#include "core/gameplay/ItemUse.hpp"

#include <algorithm>

namespace Oathbound {

    void ItemBehaviorRegistry::Register(const std::string& key, std::shared_ptr<IItemBehavior> behavior) {
        _behaviors[key] = std::move(behavior);
    }

    const IItemBehavior* ItemBehaviorRegistry::Find(const std::string& key) const {
        auto it = _behaviors.find(key);
        return it == _behaviors.end() ? nullptr : it->second.get();
    }

    namespace {
        // properties: {"heal": 25}
        class ConsumableBehavior : public IItemBehavior {
        public:
            UseResult Use(PlayerProfile& player, const ItemDefinition& definition, const ItemStack&) const override {
                const int heal = definition.Property<int>("heal", 0);
                if (heal <= 0) {
                    return {UseOutcome::NotUsable, definition.name + " has no effect."};
                }
                if (player.health >= player.maxHealth) {
                    return {UseOutcome::Failed, "Already at full health."};
                }
                const int before = player.health;
                player.health = std::min(player.maxHealth, player.health + heal);
                return {UseOutcome::Consumed,
                        "Used " + definition.name + " (+" + std::to_string(player.health - before) + " HP)."};
            }
        };
    }

    void RegisterDefaultBehaviors(ItemBehaviorRegistry& registry) {
        registry.Register("consumable", std::make_shared<ConsumableBehavior>());
    }

    UseResult UseItem(PlayerProfile& player, std::size_t slot, const IItemCatalog& catalog,
                      const ItemBehaviorRegistry& registry) {
        const ItemStack* stack = player.inventory.At(slot);
        if (!stack) return {UseOutcome::Failed, "Nothing in this slot."};

        const ItemDefinition* def = catalog.Find(stack->id);
        if (!def) return {UseOutcome::NotUsable, "Unknown item."};

        const IItemBehavior* behavior = registry.Find(def->behavior.empty() ? def->category : def->behavior);
        if (!behavior) return {UseOutcome::NotUsable, def->name + " cannot be used."};

        // Copy: the behaviour may mutate the inventory through `player`.
        const ItemStack snapshot = *stack;
        UseResult result = behavior->Use(player, *def, snapshot);
        if (result.outcome == UseOutcome::Consumed) {
            player.inventory.RemoveFromSlot(slot, 1);
        }
        return result;
    }
}
