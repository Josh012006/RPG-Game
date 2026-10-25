#include "TestFramework.hpp"
#include "TestHelpers.hpp"
#include "core/content/GameContent.hpp"
#include "core/save/SaveCodec.hpp"
#include "core/save/SaveError.hpp"
#include "core/save/SaveManager.hpp"

using namespace Oathbound;

#ifndef OATHBOUND_DATA_DIR
#error "OATHBOUND_DATA_DIR must point to the game's data/ folder"
#endif
#ifndef OATHBOUND_FIXTURES_DIR
#error "OATHBOUND_FIXTURES_DIR must point to tests/fixtures"
#endif

namespace {
    // The REAL game data: also checks the shipped JSON files are valid.
    const GameContent& Content() {
        static const GameContent content = GameContent::LoadFromDirectory(OATHBOUND_DATA_DIR);
        return content;
    }
}

TEST_CASE("content: the shipped data files load and are consistent") {
    const GameContent& c = Content();
    CHECK(c.items.Size() > 0);
    CHECK(c.worlds.All().size() > 0);
    CHECK(c.items.Find("potion_health") != nullptr);
    CHECK(c.items.Find("health_potion") == c.items.Find("potion_health"));  // alias works
    CHECK(c.worlds.Find("dark_forest") != nullptr);
    CHECK(!c.newGame.startingItems.empty());
}

TEST_CASE("content: invalid item definitions are reported, not swallowed") {
    ItemDatabase db;
    CHECK_THROWS_AS(db.LoadFromJson(Json::parse(R"({"items":[{"name":"no id"}]})")), std::runtime_error);
    CHECK_THROWS_AS(db.LoadFromJson(Json::parse(R"({"items":[{"id":"a","maxStack":0}]})")), std::runtime_error);
    CHECK_THROWS_AS(db.LoadFromJson(Json::parse(R"({"items":[{"id":"a","maxStack":"x"}]})")), std::runtime_error);
    CHECK_THROWS_AS(db.LoadFromJson(Json::parse(R"({"nope":1})")), std::runtime_error);

    ItemDatabase dup;
    CHECK_THROWS_AS(dup.LoadFromJson(Json::parse(R"({"items":[{"id":"a"},{"id":"a"}]})")), std::runtime_error);
    ItemDatabase clash;
    CHECK_THROWS_AS(clash.LoadFromJson(Json::parse(R"({"items":[{"id":"a"},{"id":"b","aliases":["a"]}]})")), std::runtime_error);
}

TEST_CASE("save: a new game writes a file with the starting inventory") {
    TempDir dir;
    SaveManager saves(dir.path, Content());

    GameSession s = saves.CreateNew("  Cedric  ");  // trimmed
    CHECK_EQ(s.player.name, std::string("Cedric"));
    CHECK_EQ(s.saveId, std::string("save_1"));
    CHECK(fs::exists(saves.PathFor("save_1")));
    CHECK_EQ(s.player.inventory.Count("potion_health"), 3);
    CHECK_EQ(s.player.inventory.Count("gold_coin"), 25);

    GameSession second = saves.CreateNew("Aria");
    CHECK_EQ(second.saveId, std::string("save_2"));
}

TEST_CASE("save: invalid names are refused") {
    TempDir dir;
    SaveManager saves(dir.path, Content());
    CHECK_THROWS_AS(saves.CreateNew(""), SaveError);
    CHECK_THROWS_AS(saves.CreateNew("     "), SaveError);
    CHECK_THROWS_AS(saves.CreateNew(std::string(21, 'x')), SaveError);
    CHECK_THROWS_AS(saves.CreateNew("tab\there"), SaveError);
    CHECK(saves.List().empty());
}

TEST_CASE("save: the whole session survives a save/load cycle") {
    TempDir dir;
    SaveManager saves(dir.path, Content());

    GameSession s = saves.CreateNew("Thorin");
    s.player.level = 7;
    s.player.health = 42;
    s.player.currentWorldId = "crystal_caves";
    s.playTimeSeconds = 1234.5;
    CHECK_EQ(s.player.inventory.Add("crystal_shard", 120), 0);   // spans two stacks (max 99)
    CHECK(s.player.inventory.Move(0, 20));                      // the player rearranged things
    // wear the sword down: per-instance state changes
    s.player.inventory.ForEach([&](std::size_t slot, const ItemStack& st) {
        if (st.id == "iron_sword") {
            auto taken = s.player.inventory.RemoveFromSlot(slot, 1);
            taken->state["durability"] = 37;
            CHECK_EQ(s.player.inventory.Add(*taken), 0);
        }
    });
    saves.Save(s);

    GameSession loaded = saves.Load(s.saveId);
    CHECK_EQ(loaded.player.name, std::string("Thorin"));
    CHECK_EQ(loaded.player.level, 7);
    CHECK_EQ(loaded.player.health, 42);
    CHECK_EQ(loaded.player.currentWorldId, std::string("crystal_caves"));
    CHECK_EQ(loaded.playTimeSeconds, 1234.5);
    CHECK_EQ(loaded.player.inventory.Count("crystal_shard"), 120);
    CHECK_EQ(loaded.player.inventory.Count("iron_sword"), 1);

    bool swordFound = false;
    loaded.player.inventory.ForEach([&](std::size_t, const ItemStack& st) {
        if (st.id == "iron_sword") { swordFound = true; CHECK_EQ(st.state["durability"].get<int>(), 37); }
    });
    CHECK(swordFound);

    // exact same layout, slot by slot
    for (std::size_t i = 0; i < s.player.inventory.Capacity(); ++i) {
        const ItemStack* a = s.player.inventory.At(i);
        const ItemStack* b = loaded.player.inventory.At(i);
        CHECK((a == nullptr) == (b == nullptr));
        if (a && b) CHECK(*a == *b);
    }
}

TEST_CASE("save: loading does not touch the file and writing leaves no temp file") {
    TempDir dir;
    SaveManager saves(dir.path, Content());
    GameSession s = saves.CreateNew("Luna");
    saves.Save(s);

    for (const auto& e : fs::directory_iterator(dir.path)) {
        CHECK(e.path().extension() != ".tmp");
    }
    const auto before = fs::last_write_time(saves.PathFor(s.saveId));
    (void)saves.Load(s.saveId);
    CHECK(fs::last_write_time(saves.PathFor(s.saveId)) == before);
}

TEST_CASE("save: a corrupt file is restored from the automatic backup") {
    TempDir dir;
    SaveManager saves(dir.path, Content());

    GameSession s = saves.CreateNew("Josh");
    s.player.level = 3;
    saves.Save(s);          // .bak now holds the level-1 version... 
    s.player.level = 4;
    saves.Save(s);          // ...and now the level-3 version

    WriteText(saves.PathFor(s.saveId), "{ this is not json");   // e.g. power cut / disk problem

    GameSession recovered = saves.Load(s.saveId);
    CHECK_EQ(recovered.player.level, 3);                         // last good version
    CHECK(fs::exists(saves.PathFor(s.saveId).string() + ".corrupt"));

    // the main file is healthy again, and the next save does not destroy the good backup
    GameSession again = saves.Load(s.saveId);
    CHECK_EQ(again.player.level, 3);
}

TEST_CASE("save: a corrupt file without a usable backup is reported, never half-loaded") {
    TempDir dir;
    SaveManager saves(dir.path, Content());
    WriteText(saves.PathFor("save_9"), "garbage");
    CHECK_THROWS_AS(saves.Load("save_9"), SaveError);

    const auto list = saves.List();
    CHECK_EQ(list.size(), std::size_t(1));
    CHECK(!list[0].valid);
    CHECK(!list[0].error.empty());
}

TEST_CASE("save: a save from a NEWER game version is refused, not recovered from the backup") {
    TempDir dir;
    SaveManager saves(dir.path, Content());
    GameSession s = saves.CreateNew("Future");
    saves.Save(s);  // creates a valid .bak

    WriteText(saves.PathFor(s.saveId), R"({"version": 99, "player": {"name": "Future"}})");
    CHECK_THROWS_AS(saves.Load(s.saveId), SaveTooNewError);
}

TEST_CASE("save: ids cannot escape the save directory") {
    TempDir dir;
    SaveManager saves(dir.path, Content());
    CHECK_THROWS_AS(saves.Load("../secret"), SaveError);
    CHECK_THROWS_AS(saves.Load("a/b"), SaveError);
    CHECK_THROWS_AS(saves.Load(""), SaveError);
    CHECK(!saves.Delete("../secret"));
}

TEST_CASE("save: List is sorted newest first and Delete removes the save and its backup") {
    TempDir dir;
    SaveManager saves(dir.path, Content());
    GameSession a = saves.CreateNew("Old");
    GameSession b = saves.CreateNew("New");
    // force distinct timestamps without sleeping
    WriteText(saves.PathFor(a.saveId), EncodeSession(a).dump());
    a.updatedAt = "2020-01-01T00:00:00Z";
    WriteJsonFileAtomic(saves.PathFor(a.saveId), EncodeSession(a), false);
    b.updatedAt = "2030-01-01T00:00:00Z";
    WriteJsonFileAtomic(saves.PathFor(b.saveId), EncodeSession(b), false);

    auto list = saves.List();
    CHECK_EQ(list.size(), std::size_t(2));
    CHECK_EQ(list[0].playerName, std::string("New"));
    CHECK_EQ(list[1].playerName, std::string("Old"));

    CHECK(saves.Delete(a.saveId));
    CHECK_EQ(saves.List().size(), std::size_t(1));
}

TEST_CASE("migration: the real v1 prototype saves are imported, world names become ids") {
    TempDir dir;
    const fs::path legacy = dir.path / "data";
    fs::create_directories(legacy);
    for (const auto& e : fs::directory_iterator(fs::path(OATHBOUND_FIXTURES_DIR) / "legacy")) {
        fs::copy_file(e.path(), legacy / e.path().filename());
    }

    SaveManager saves(dir.path / "saves", Content());
    CHECK_EQ(saves.ImportLegacySaves(legacy), 6);
    CHECK_EQ(saves.ImportLegacySaves(legacy), 0);  // idempotent: originals were renamed
    CHECK(fs::exists(legacy / "saveData_2.json.migrated"));
    CHECK(!fs::exists(legacy / "saveData_2.json"));

    bool foundAria = false, foundJosh = false;
    for (const auto& s : saves.List()) {
        CHECK(s.valid);
        if (s.playerName == "Aria") {
            foundAria = true;
            CHECK_EQ(s.level, 5);
            CHECK_EQ(s.worldName, std::string("The Crystal Caves"));
            GameSession g = saves.Load(s.id);
            CHECK_EQ(g.player.currentWorldId, std::string("crystal_caves"));
            CHECK(g.player.inventory.IsEmpty());   // v1 had no inventory
            CHECK_EQ(g.player.inventory.Capacity(), Content().newGame.inventoryCapacity);
        }
        if (s.playerName == "Josh") {              // v1 save with no world at all
            foundJosh = true;
            CHECK_EQ(s.worldName, std::string("No world selected"));
        }
    }
    CHECK(foundAria);
    CHECK(foundJosh);
}

TEST_CASE("migration: v1 with an unknown world name still imports") {
    Json v1 = Json::parse(R"({"player":{"name":"X","level":2,"currentWorld":"A World That Was Removed"}})");
    Json v2 = MigrateSaveData(v1, Content().worlds);
    CHECK_EQ(v2["version"].get<int>(), 2);
    CHECK(!v2["player"].contains("currentWorld"));
    GameSession s = DecodeSession(v2, Content());
    CHECK_EQ(s.player.currentWorldId, std::string(""));
}

TEST_CASE("codec: decode is lenient about missing optional fields but not about identity") {
    GameSession ok = DecodeSession(Json::parse(R"({"version":2,"player":{"name":"Min"}})"), Content());
    CHECK_EQ(ok.player.level, 1);
    CHECK_EQ(ok.player.health, ok.player.maxHealth);

    CHECK_THROWS_AS(DecodeSession(Json::parse(R"({"version":2})"), Content()), SaveError);
    CHECK_THROWS_AS(DecodeSession(Json::parse(R"({"version":2,"player":{"level":3}})"), Content()), SaveError);

    GameSession clamped = DecodeSession(Json::parse(R"({"player":{"name":"H","health":9999,"maxHealth":50,"level":-4}})"), Content());
    CHECK_EQ(clamped.player.health, 50);
    CHECK_EQ(clamped.player.level, 1);
}

TEST_CASE("items: using a consumable heals, is consumed, and refuses when pointless") {
    const GameContent& c = Content();
    PlayerProfile p(c.items, "Tester");
    CHECK_EQ(p.inventory.Add("potion_health", 2), 0);
    CHECK_EQ(p.inventory.Add("iron_sword"), 0);
    CHECK_EQ(p.inventory.Add("rusty_key"), 0);

    p.health = p.maxHealth;
    auto full = UseItem(p, 0, c.items, c.behaviors);
    CHECK(full.outcome == UseOutcome::Failed);
    CHECK_EQ(p.inventory.Count("potion_health"), 2);   // not consumed

    p.health = 50;
    auto used = UseItem(p, 0, c.items, c.behaviors);
    CHECK(used.outcome == UseOutcome::Consumed);
    CHECK_EQ(p.health, 75);
    CHECK_EQ(p.inventory.Count("potion_health"), 1);

    p.health = p.maxHealth - 5;                         // heal is capped at maxHealth
    CHECK(UseItem(p, 0, c.items, c.behaviors).outcome == UseOutcome::Consumed);
    CHECK_EQ(p.health, p.maxHealth);
    CHECK_EQ(p.inventory.Count("potion_health"), 0);

    CHECK(UseItem(p, 1, c.items, c.behaviors).outcome == UseOutcome::NotUsable);   // weapon: no behaviour
    CHECK(UseItem(p, 2, c.items, c.behaviors).outcome == UseOutcome::NotUsable);   // quest item
    CHECK(UseItem(p, 15, c.items, c.behaviors).outcome == UseOutcome::Failed);     // empty slot
}

TEST_CASE("worlds: lock state is derived from the player's level") {
    const WorldDatabase& w = Content().worlds;
    CHECK(w.Find("dark_forest")->IsUnlockedFor(1));
    CHECK(!w.Find("sky_islands")->IsUnlockedFor(4));
    CHECK(w.Find("sky_islands")->IsUnlockedFor(5));
    CHECK(w.FindByName("The Dragon's Peak") != nullptr);
    CHECK_EQ(*w.IndexOf("crystal_caves"), std::size_t(1));
    CHECK(!w.IndexOf("nowhere").has_value());
}
