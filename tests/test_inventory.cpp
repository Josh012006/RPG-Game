#include "TestFramework.hpp"
#include "TestHelpers.hpp"
#include "core/inventory/Inventory.hpp"

using namespace Oathbound;

TEST_CASE("inventory: add fills stacks then slots and returns the remainder") {
    auto db = MakeTestItems();
    Inventory inv(db, 2);  // 2 slots x maxStack 10

    CHECK_EQ(inv.Add("potion", 15), 0);
    CHECK_EQ(inv.At(0)->quantity, 10);
    CHECK_EQ(inv.At(1)->quantity, 5);

    CHECK_EQ(inv.Add("potion", 10), 5);  // only 5 more fit
    CHECK_EQ(inv.Count("potion"), 20);
    CHECK_EQ(inv.FreeSlots(), std::size_t(0));
}

TEST_CASE("inventory: unknown items are rejected, nothing changes") {
    auto db = MakeTestItems();
    Inventory inv(db);
    const auto rev = inv.Revision();
    CHECK_EQ(inv.Add("does_not_exist", 3), 3);
    CHECK(inv.IsEmpty());
    CHECK_EQ(inv.Revision(), rev);
}

TEST_CASE("inventory: add resolves aliases to the canonical id") {
    auto db = MakeTestItems();
    Inventory inv(db);
    CHECK_EQ(inv.Add("old_potion", 2), 0);
    CHECK_EQ(inv.At(0)->id, std::string("potion"));
    CHECK_EQ(inv.Count("old_potion"), 2);  // queries accept aliases too
}

TEST_CASE("inventory: per-instance state keeps stacks apart") {
    auto db = MakeTestItems();
    Inventory inv(db);
    CHECK_EQ(inv.Add(ItemStack("herb", 5, Json{{"quality", "fine"}})), 0);
    CHECK_EQ(inv.Add("herb", 5), 0);
    CHECK_EQ(inv.UsedSlots(), std::size_t(2));  // not merged: different state
    CHECK_EQ(inv.Count("herb"), 10);

    CHECK_EQ(inv.Add("herb", 5), 0);            // plain herbs merge with the plain stack
    CHECK_EQ(inv.UsedSlots(), std::size_t(2));
    CHECK_EQ(inv.At(1)->quantity, 10);
}

TEST_CASE("inventory: defaultState is applied by Add(id, qty)") {
    auto db = MakeTestItems();
    Inventory inv(db);
    CHECK_EQ(inv.Add("sword"), 0);
    CHECK_EQ(inv.At(0)->state["durability"].get<int>(), 100);
}

TEST_CASE("inventory: an empty state object equals no state") {
    auto db = MakeTestItems();
    Inventory inv(db);
    CHECK_EQ(inv.Add(ItemStack("herb", 3, Json::object())), 0);
    CHECK_EQ(inv.Add(ItemStack("herb", 3, Json())), 0);
    CHECK_EQ(inv.UsedSlots(), std::size_t(1));
}

TEST_CASE("inventory: remove is all-or-nothing and spans stacks") {
    auto db = MakeTestItems();
    Inventory inv(db);
    CHECK_EQ(inv.Add("herb", 150), 0);  // 99 + 51

    CHECK(!inv.Remove("herb", 151));
    CHECK_EQ(inv.Count("herb"), 150);   // untouched

    CHECK(inv.Remove("herb", 100));
    CHECK_EQ(inv.Count("herb"), 50);
    CHECK(inv.Has("herb", 50));
    CHECK(!inv.Has("herb", 51));
    CHECK_EQ(inv.UsedSlots(), std::size_t(1));  // emptied slots are freed
}

TEST_CASE("inventory: RemoveFromSlot takes part of a stack") {
    auto db = MakeTestItems();
    Inventory inv(db);
    CHECK_EQ(inv.Add("potion", 6), 0);

    auto taken = inv.RemoveFromSlot(0, 4);
    CHECK(taken.has_value());
    CHECK_EQ(taken->quantity, 4);
    CHECK_EQ(inv.At(0)->quantity, 2);

    taken = inv.RemoveFromSlot(0, 99);  // more than available: takes what is there
    CHECK_EQ(taken->quantity, 2);
    CHECK(inv.At(0) == nullptr);
    CHECK(!inv.RemoveFromSlot(0, 1).has_value());
    CHECK(!inv.RemoveFromSlot(999, 1).has_value());
}

TEST_CASE("inventory: Move relocates, merges and swaps") {
    auto db = MakeTestItems();
    Inventory inv(db, 5);
    CHECK_EQ(inv.Add("potion", 10), 0);  // slot 0 (full)
    CHECK_EQ(inv.Add("herb", 3), 0);     // slot 1
    CHECK_EQ(inv.Add("potion", 4), 0);   // slot 2

    CHECK(inv.Move(1, 4));               // to an empty slot
    CHECK(inv.At(1) == nullptr);
    CHECK_EQ(inv.At(4)->id, std::string("herb"));

    CHECK(inv.Move(4, 2));               // different kinds: swap
    CHECK_EQ(inv.At(2)->id, std::string("herb"));
    CHECK_EQ(inv.At(4)->id, std::string("potion"));
    CHECK_EQ(inv.At(4)->quantity, 4);

    CHECK(!inv.Move(4, 0));              // same kind but target (slot 0) is full: nothing to do
    CHECK(!inv.Move(3, 4));              // empty source
    CHECK(!inv.Move(0, 99));             // out of range
    CHECK(!inv.Move(4, 4));              // same slot

    CHECK(inv.Move(0, 4));               // merge: slot 4 had room for 6 more
    CHECK_EQ(inv.At(4)->quantity, 10);
    CHECK_EQ(inv.At(0)->quantity, 4);
    CHECK(!inv.Move(0, 4));              // slot 4 is full now
    CHECK_EQ(inv.Count("potion"), 14);   // merging never creates or destroys items
}

TEST_CASE("inventory: Sort merges partial stacks and orders by category") {
    auto db = MakeTestItems();
    Inventory inv(db, 8);
    CHECK_EQ(inv.Add("potion", 10), 0);  // slot 0 (full)
    CHECK_EQ(inv.Add("herb", 5), 0);     // slot 1
    CHECK_EQ(inv.Add("sword"), 0);       // slot 2
    CHECK_EQ(inv.Add("potion", 3), 0);   // slot 3 (slot 0 is full)
    CHECK(inv.RemoveFromSlot(0, 4).has_value());  // slot 0 now holds 6 -> two partial potion stacks

    inv.Sort();

    CHECK_EQ(inv.Count("potion"), 9);
    CHECK_EQ(inv.Count("herb"), 5);
    // categories sort as: consumable < material < weapon
    CHECK_EQ(inv.At(0)->id, std::string("potion"));
    CHECK_EQ(inv.At(0)->quantity, 9);    // the two partial stacks were merged
    CHECK_EQ(inv.At(1)->id, std::string("herb"));
    CHECK_EQ(inv.At(2)->id, std::string("sword"));
    CHECK(inv.At(3) == nullptr);
}

TEST_CASE("inventory: capacity can grow, and only shrink over empty slots") {
    auto db = MakeTestItems();
    Inventory inv(db, 4);
    CHECK_EQ(inv.Add("key"), 0);
    CHECK(inv.Move(0, 3));
    CHECK(!inv.SetCapacity(3));  // slot 3 is occupied
    CHECK_EQ(inv.Capacity(), std::size_t(4));
    CHECK(inv.Move(3, 0));
    CHECK(inv.SetCapacity(3));
    CHECK(inv.SetCapacity(30));
    CHECK_EQ(inv.Capacity(), std::size_t(30));
}

TEST_CASE("inventory: RoomFor predicts what Add would accept") {
    auto db = MakeTestItems();
    Inventory inv(db, 2);
    CHECK_EQ(inv.Add("potion", 7), 0);
    CHECK_EQ(inv.RoomFor(ItemStack("potion", 1)), 13);  // 3 in the stack + 10 in the free slot
    CHECK_EQ(inv.RoomFor(ItemStack("nope", 1)), 0);
}
