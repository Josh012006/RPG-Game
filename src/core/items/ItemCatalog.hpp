#pragma once

#include "core/items/ItemDefinition.hpp"

namespace Oathbound {

    // Read-only view of "which item kinds exist". The inventory depends on this tiny interface
    // only, never on the concrete database: it can be tested with a fake, and the real data can
    // come from JSON, a mod loader, a network sync...
    class IItemCatalog {
    public:
        virtual ~IItemCatalog() = default;

        // Returns nullptr if the id (or alias) is unknown. The pointer stays valid as long as
        // the catalog is not modified.
        virtual const ItemDefinition* Find(const ItemId& id) const = 0;

        // Maps an old alias to the current id; unknown ids are returned unchanged.
        virtual ItemId Canonicalize(const ItemId& id) const { return id; }
    };
}
