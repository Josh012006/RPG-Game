#include "core/inventory/Inventory.hpp"

#include <algorithm>
#include <iterator>
#include <limits>
#include <tuple>

#include "core/util/Log.hpp"

namespace Oathbound {

    Inventory::Inventory(const IItemCatalog& catalog, std::size_t capacity)
        : _catalog(&catalog), _slots(std::min(capacity, kMaxCapacity)) {}

    int Inventory::MaxStackOf(const ItemId& id) const {
        const ItemDefinition* def = _catalog->Find(id);
        return def ? def->maxStack : 1;
    }

    std::size_t Inventory::UsedSlots() const {
        return static_cast<std::size_t>(std::count_if(_slots.begin(), _slots.end(),
                                                      [](const auto& s) { return s.has_value(); }));
    }

    const ItemStack* Inventory::At(std::size_t slot) const {
        if (slot >= _slots.size() || !_slots[slot]) return nullptr;
        return &*_slots[slot];
    }

    int Inventory::Count(const ItemId& rawId) const {
        const ItemId id = _catalog->Canonicalize(rawId);
        long long total = 0;
        for (const auto& slot : _slots) {
            if (slot && slot->id == id) total += slot->quantity;
        }
        return static_cast<int>(std::min<long long>(total, std::numeric_limits<int>::max()));
    }

    int Inventory::RoomFor(const ItemStack& stack) const {
        const ItemDefinition* def = _catalog->Find(stack.id);
        if (!def) return 0;
        ItemStack probe(def->id, 1, stack.state);

        long long room = 0;
        for (const auto& slot : _slots) {
            if (!slot) {
                room += def->maxStack;
            } else if (slot->SameKindAs(probe) && slot->quantity < def->maxStack) {
                room += def->maxStack - slot->quantity;
            }
        }
        return static_cast<int>(std::min<long long>(room, std::numeric_limits<int>::max()));
    }

    // ---- Mutations ---------------------------------------------------------------------

    int Inventory::Add(ItemStack stack) {
        if (stack.quantity <= 0) return 0;

        const ItemDefinition* def = _catalog->Find(stack.id);
        if (!def) {
            log::Warn("Inventory::Add: unknown item '" + stack.id + "' rejected");
            return stack.quantity;
        }
        stack.id = def->id;  // canonical id (resolves aliases)

        const int maxStack = def->maxStack;
        int remaining = stack.quantity;

        // 1) top up existing stacks of the same kind
        for (auto& slot : _slots) {
            if (remaining == 0) break;
            if (slot && slot->SameKindAs(stack) && slot->quantity < maxStack) {
                const int n = std::min(maxStack - slot->quantity, remaining);
                slot->quantity += n;
                remaining -= n;
            }
        }
        // 2) then fill empty slots
        for (auto& slot : _slots) {
            if (remaining == 0) break;
            if (!slot) {
                const int n = std::min(maxStack, remaining);
                slot = ItemStack(stack.id, n, stack.state);
                remaining -= n;
            }
        }

        if (remaining != stack.quantity) ++_revision;
        return remaining;
    }

    int Inventory::Add(const ItemId& id, int quantity) {
        const ItemDefinition* def = _catalog->Find(id);
        if (!def) {
            log::Warn("Inventory::Add: unknown item '" + id + "' rejected");
            return quantity;
        }
        return Add(ItemStack(def->id, quantity, def->defaultState));
    }

    bool Inventory::Remove(const ItemId& rawId, int quantity) {
        if (quantity <= 0) return true;
        const ItemId id = _catalog->Canonicalize(rawId);
        if (Count(id) < quantity) return false;

        int remaining = quantity;
        for (auto it = _slots.rbegin(); it != _slots.rend() && remaining > 0; ++it) {
            if (!*it || (*it)->id != id) continue;
            const int n = std::min((*it)->quantity, remaining);
            (*it)->quantity -= n;
            remaining -= n;
            if ((*it)->quantity <= 0) it->reset();
        }
        ++_revision;
        return true;
    }

    std::optional<ItemStack> Inventory::RemoveFromSlot(std::size_t slot, int quantity) {
        if (quantity <= 0 || slot >= _slots.size() || !_slots[slot]) return std::nullopt;

        ItemStack& source = *_slots[slot];
        const int n = std::min(quantity, source.quantity);
        ItemStack taken(source.id, n, source.state);
        source.quantity -= n;
        if (source.quantity <= 0) _slots[slot].reset();
        ++_revision;
        return taken;
    }

    bool Inventory::Move(std::size_t from, std::size_t to) {
        if (from == to || from >= _slots.size() || to >= _slots.size() || !_slots[from]) return false;

        auto& source = _slots[from];
        auto& target = _slots[to];

        if (!target) {
            target = std::move(source);
            source.reset();
        } else if (source->SameKindAs(*target)) {
            const int room = std::max(0, MaxStackOf(target->id) - target->quantity);
            const int n = std::min(room, source->quantity);
            if (n == 0) return false;  // target already full: nothing to do
            target->quantity += n;
            source->quantity -= n;
            if (source->quantity <= 0) source.reset();
        } else {
            std::swap(source, target);
        }
        ++_revision;
        return true;
    }

    void Inventory::Sort() {
        // 1) merge partial stacks of the same kind
        std::vector<ItemStack> merged;
        for (auto& slot : _slots) {
            if (!slot) continue;
            ItemStack stack = std::move(*slot);
            const int maxStack = MaxStackOf(stack.id);
            for (auto& m : merged) {
                if (stack.quantity <= 0) break;
                if (m.SameKindAs(stack) && m.quantity < maxStack) {
                    const int n = std::min(maxStack - m.quantity, stack.quantity);
                    m.quantity += n;
                    stack.quantity -= n;
                }
            }
            if (stack.quantity > 0) merged.push_back(std::move(stack));
        }

        // 2) order by category, name, id (unknown items go last)
        auto key = [this](const ItemStack& s) {
            const ItemDefinition* def = _catalog->Find(s.id);
            return std::make_tuple(def ? def->category : std::string("~"),
                                   def ? def->name : s.id, s.id);
        };
        std::stable_sort(merged.begin(), merged.end(),
                         [&](const ItemStack& a, const ItemStack& b) { return key(a) < key(b); });

        // 3) pack to the front
        for (auto& slot : _slots) slot.reset();
        for (std::size_t i = 0; i < merged.size(); ++i) _slots[i] = std::move(merged[i]);
        ++_revision;
    }

    void Inventory::Clear() {
        for (auto& slot : _slots) slot.reset();
        ++_revision;
    }

    bool Inventory::SetCapacity(std::size_t capacity) {
        if (capacity > kMaxCapacity) return false;
        for (std::size_t i = capacity; i < _slots.size(); ++i) {
            if (_slots[i]) return false;
        }
        _slots.resize(capacity);
        ++_revision;
        return true;
    }

    // ---- Persistence -------------------------------------------------------------------

    Json Inventory::ToJson() const {
        Json slots = Json::array();
        for (std::size_t i = 0; i < _slots.size(); ++i) {
            if (!_slots[i]) continue;
            Json entry = Json::object();
            entry["slot"] = i;
            entry["id"] = _slots[i]->id;
            entry["qty"] = _slots[i]->quantity;
            if (!IsEmptyState(_slots[i]->state)) entry["state"] = _slots[i]->state;
            slots.push_back(std::move(entry));
        }
        Json out = Json::object();
        out["capacity"] = _slots.size();
        out["slots"] = std::move(slots);
        return out;
    }

    void Inventory::LoadJson(const Json& json, std::vector<std::string>* warnings) {
        auto warn = [&](const std::string& message) {
            log::Warn("inventory: " + message);
            if (warnings) warnings->push_back(message);
        };

        const std::size_t previousCapacity = _slots.size();
        _slots.assign(previousCapacity, std::nullopt);
        ++_revision;

        if (!json.is_object()) {
            warn("saved inventory is not an object, starting empty");
            return;
        }

        // Capacity
        std::size_t capacity = previousCapacity;
        if (auto it = json.find("capacity"); it != json.end() && it->is_number_integer()) {
            const long long saved = it->get<long long>();
            if (saved > 0) capacity = static_cast<std::size_t>(std::min<long long>(saved, kMaxCapacity));
            if (saved > static_cast<long long>(kMaxCapacity)) warn("capacity clamped to " + std::to_string(kMaxCapacity));
        }
        _slots.assign(capacity, std::nullopt);

        // Parse entries
        struct Entry { ItemStack stack; long long slot; };
        std::vector<Entry> entries;

        auto slotsIt = json.find("slots");
        if (slotsIt == json.end() || !slotsIt->is_array()) {
            if (slotsIt != json.end()) warn("'slots' is not an array, ignored");
            return;
        }

        for (const auto& e : *slotsIt) {
            if (!e.is_object()) { warn("slot entry is not an object, skipped"); continue; }

            auto idIt = e.find("id");
            if (idIt == e.end() || !idIt->is_string() || idIt->get<std::string>().empty()) {
                warn("slot entry without a valid 'id', skipped");
                continue;
            }
            auto qtyIt = e.find("qty");
            if (qtyIt == e.end() || !qtyIt->is_number_integer() || qtyIt->get<long long>() < 1) {
                warn("item '" + idIt->get<std::string>() + "' has an invalid quantity, skipped");
                continue;
            }

            ItemId id = _catalog->Canonicalize(idIt->get<std::string>());
            if (!_catalog->Find(id)) {
                warn("unknown item '" + id + "' kept as-is");
            }

            Entry entry;
            entry.stack = ItemStack(id,
                                    static_cast<int>(std::min<long long>(qtyIt->get<long long>(), std::numeric_limits<int>::max())),
                                    e.contains("state") ? e["state"] : Json());
            entry.slot = -1;
            if (auto slotIt = e.find("slot"); slotIt != e.end() && slotIt->is_number_integer()) {
                entry.slot = slotIt->get<long long>();
            }
            entries.push_back(std::move(entry));
        }

        // Pass 1: honour the saved positions. Pass 2: relocate whatever could not be placed.
        std::vector<ItemStack> displaced;
        for (auto& entry : entries) {
            const bool inRange = entry.slot >= 0 && entry.slot < static_cast<long long>(_slots.size());
            if (inRange && !_slots[static_cast<std::size_t>(entry.slot)]) {
                _slots[static_cast<std::size_t>(entry.slot)] = std::move(entry.stack);
            } else {
                displaced.push_back(std::move(entry.stack));
            }
        }
        for (auto& stack : displaced) {
            auto freeSlot = std::find_if(_slots.begin(), _slots.end(), [](const auto& s) { return !s.has_value(); });
            if (freeSlot == _slots.end()) {
                _slots.emplace_back();  // never drop a valid item: grow instead
                freeSlot = std::prev(_slots.end());
                warn("inventory grown to " + std::to_string(_slots.size()) + " slots to keep '" + stack.id + "'");
            } else {
                warn("item '" + stack.id + "' relocated (its saved slot was unavailable)");
            }
            *freeSlot = std::move(stack);
        }
    }
}
