#include "TestFramework.hpp"
#include "TestHelpers.hpp"
#include "core/inventory/Inventory.hpp"

using namespace Oathbound;

namespace {
    // Same slots, same stacks (id, quantity, state), same capacity.
    bool SameContent(const Inventory& a, const Inventory& b) {
        if (a.Capacity() != b.Capacity()) return false;
        for (std::size_t i = 0; i < a.Capacity(); ++i) {
            const ItemStack* x = a.At(i);
            const ItemStack* y = b.At(i);
            if ((x == nullptr) != (y == nullptr)) return false;
            if (x && *x != *y) return false;
        }
        return true;
    }
}

TEST_CASE("persistence: save then load restores everything, including slot positions and state") {
    auto db = MakeTestItems();
    Inventory original(db, 12);
    CHECK_EQ(original.Add("potion", 14), 0);
    CHECK_EQ(original.Add("sword"), 0);
    CHECK_EQ(original.Add(ItemStack("herb", 7, Json{{"quality", "fine"}, {"tags", {"a", "b"}}})), 0);
    CHECK(original.Move(1, 9));  // leave gaps: the player's layout must survive
    CHECK(original.Move(0, 6));

    const Json saved = original.ToJson();

    Inventory restored(db, 1);  // capacity comes from the save, not from the constructor
    restored.LoadJson(saved);

    CHECK(SameContent(original, restored));
    CHECK_EQ(restored.At(2)->id, std::string("sword"));
    CHECK_EQ(restored.At(2)->state["durability"].get<int>(), 100);
    CHECK_EQ(restored.At(9)->id, std::string("potion"));       // the stack the player dragged to slot 9
    CHECK_EQ(restored.At(3)->state["quality"].get<std::string>(), std::string("fine"));

    // and the JSON text itself is stable (no drift after repeated save/load cycles)
    CHECK(restored.ToJson() == saved);
}

TEST_CASE("persistence: output only lists occupied slots") {
    auto db = MakeTestItems();
    Inventory inv(db, 100);
    CHECK_EQ(inv.Add("key"), 0);
    const Json j = inv.ToJson();
    CHECK_EQ(j["capacity"].get<int>(), 100);
    CHECK_EQ(j["slots"].size(), std::size_t(1));
    CHECK(!j["slots"][0].contains("state"));  // no empty "state" noise
}

TEST_CASE("persistence: items whose definition disappeared are KEPT and re-saved untouched") {
    auto db = MakeTestItems();
    Inventory inv(db, 4);
    std::vector<std::string> warnings;
    inv.LoadJson(Json::parse(R"({"capacity":4,"slots":[
        {"slot":0,"id":"removed_in_update_1_2","qty":3,"state":{"rune":"fire"}},
        {"slot":2,"id":"potion","qty":2}]})"), &warnings);

    CHECK_EQ(inv.UsedSlots(), std::size_t(2));
    CHECK_EQ(inv.At(0)->id, std::string("removed_in_update_1_2"));
    CHECK_EQ(warnings.size(), std::size_t(1));

    const Json resaved = inv.ToJson();
    CHECK_EQ(resaved["slots"][0]["id"].get<std::string>(), std::string("removed_in_update_1_2"));
    CHECK_EQ(resaved["slots"][0]["qty"].get<int>(), 3);
    CHECK_EQ(resaved["slots"][0]["state"]["rune"].get<std::string>(), std::string("fire"));

    // the unknown stack must not be consumed by normal operations either
    CHECK_EQ(inv.Add("potion", 1), 0);
    CHECK_EQ(inv.At(0)->quantity, 3);
}

TEST_CASE("persistence: renamed items load through their aliases") {
    auto db = MakeTestItems();
    Inventory inv(db);
    inv.LoadJson(Json::parse(R"({"capacity":24,"slots":[{"slot":0,"id":"old_potion","qty":4}]})"));
    CHECK_EQ(inv.At(0)->id, std::string("potion"));
    CHECK_EQ(inv.Count("potion"), 4);
}

TEST_CASE("persistence: unusable entries are skipped, valid ones survive") {
    auto db = MakeTestItems();
    Inventory inv(db);
    std::vector<std::string> warnings;
    inv.LoadJson(Json::parse(R"({"capacity":6,"slots":[
        {"slot":0,"id":"potion","qty":0},
        {"slot":1,"qty":3},
        {"slot":2,"id":"","qty":3},
        {"slot":3,"id":"herb","qty":"many"},
        42,
        {"slot":4,"id":"key","qty":1}]})"), &warnings);

    CHECK_EQ(inv.UsedSlots(), std::size_t(1));
    CHECK_EQ(inv.At(4)->id, std::string("key"));
    CHECK_EQ(warnings.size(), std::size_t(5));
}

TEST_CASE("persistence: colliding or out-of-range slots are relocated, nothing is lost") {
    auto db = MakeTestItems();
    Inventory inv(db);
    inv.LoadJson(Json::parse(R"({"capacity":4,"slots":[
        {"slot":1,"id":"herb","qty":5},
        {"slot":1,"id":"key","qty":1},
        {"slot":99,"id":"potion","qty":2},
        {"slot":-3,"id":"sword","qty":1},
        {"id":"herb","qty":9,"state":{"x":1}}]})"));

    CHECK_EQ(inv.Count("herb"), 14);
    CHECK_EQ(inv.Count("key"), 1);
    CHECK_EQ(inv.Count("potion"), 2);
    CHECK_EQ(inv.Count("sword"), 1);
    CHECK_EQ(inv.At(1)->id, std::string("herb"));       // first claim on slot 1 wins
    CHECK_EQ(inv.Capacity(), std::size_t(5));            // grew by one to keep all 5 stacks
}

TEST_CASE("persistence: stacks above maxStack (content was rebalanced) are tolerated") {
    auto db = MakeTestItems();  // potion maxStack = 10
    Inventory inv(db);
    inv.LoadJson(Json::parse(R"({"slots":[{"slot":0,"id":"potion","qty":50}]})"));
    CHECK_EQ(inv.At(0)->quantity, 50);
    CHECK_EQ(inv.ToJson()["slots"][0]["qty"].get<int>(), 50);
    CHECK_EQ(inv.Add("potion", 1), 0);  // new potions go to a new slot, the big stack is not touched
    CHECK_EQ(inv.At(0)->quantity, 50);
    CHECK_EQ(inv.At(1)->quantity, 1);
}

TEST_CASE("persistence: garbage input never throws and never crashes") {
    auto db = MakeTestItems();
    Inventory inv(db, 5);
    CHECK_EQ(inv.Add("key"), 0);

    for (const char* text : {"null", "[]", "\"x\"", "123", "{}", R"({"slots":"nope"})",
                             R"({"capacity":"big","slots":[]})", R"({"capacity":-5})",
                             R"({"capacity":99999999999999})"}) {
        inv.LoadJson(Json::parse(text));
        CHECK(inv.Capacity() >= 1);
        CHECK(inv.Capacity() <= Inventory::kMaxCapacity);
    }
}
