# Testing

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · Related: [Building](building.md) · [Inventory](../features/inventory.md#tests) · [Save system](../features/save-system.md)

Automated tests cover **`src/core`**: items, inventory, saves, migration, item use and worlds. They need no window and no graphics driver.
`engine/` and `states/` are not unit-tested.

## Running

```bash
cmake --build build --config Release --target oathbound_tests
ctest --test-dir build -C Release --output-on-failure
# or run the executable directly for per-test output:
./bin/oathbound_tests
```

Output lists each test as `[ ok ]` or `[FAIL]`, then a summary (`36 tests, 0 failed`). A non-zero exit code means a failure.

## What is covered

| File | Area | Tests |
|---|---|---|
| `tests/test_inventory.cpp` | Add/remove/move/sort/capacity | stacking and remainder, unknown items, aliases, per-instance state, `defaultState`, `Remove` across stacks, `RemoveFromSlot`, `Move`, `Sort`, `SetCapacity`, `RoomFor` |
| `tests/test_inventory_persistence.cpp` | Inventory save/load | round-trip, sparse output, unknown items kept, aliases, invalid entries, slot collisions, over-sized stacks, garbage input |
| `tests/test_save.cpp` | Content, saves, migration, item use, worlds | shipped data loads, invalid definitions rejected, create/load round-trip, name validation, backup recovery, corrupt file, too-new version, id safety, listing and delete, **real prototype saves migrated**, lenient decode, item use, unlock rule |

Details by feature: [Inventory](../features/inventory.md#tests), [Save system](../features/save-system.md#guarantees).

## The test framework

`tests/TestFramework.hpp` is a tiny dependency-free harness. The macros have the same shape as doctest or Catch2, so it can be replaced later.

| Macro | Purpose |
|---|---|
| `TEST_CASE("name") { ... }` | Defines and **auto-registers** a test |
| `CHECK(expr)` | Records a failure (with file and line) if false; the test continues |
| `CHECK_EQ(actual, expected)` | Compares and prints both values on failure (values are copied, so temporaries are safe) |
| `CHECK_THROWS_AS(expr, Type)` | Fails unless `expr` throws `Type` |

`tests/TestMain.cpp` runs every registered test; an uncaught exception is reported as a failure.

## Helpers

`tests/TestHelpers.hpp`:

| Helper | Purpose |
|---|---|
| `MakeTestItems()` | A small catalogue built from inline JSON (`potion` max 10 with alias `old_potion`, `sword` with `defaultState`, `herb` max 99, `key`). Inventory tests use it so they do not depend on `data/items.json` |
| `TempDir` | RAII temporary folder, deleted when it goes out of scope |
| `WriteText(path, text)` | Writes a file (used to create corrupt saves) |

## Writing a test

```cpp
#include "TestFramework.hpp"
#include "TestHelpers.hpp"
#include "core/inventory/Inventory.hpp"
using namespace Oathbound;

TEST_CASE("inventory: describe the behaviour in one line") {
    auto db = MakeTestItems();
    Inventory inv(db, 4);

    CHECK_EQ(inv.Add("potion", 12), 0);          // 10 + 2
    CHECK_EQ(inv.Count("potion"), 12);
}
```

* Name tests `"area: what is guaranteed"`.
* New `tests/test_*.cpp` files are picked up automatically by CMake (re-run `cmake` if needed).
* Prefer `MakeTestItems()` over the shipped data for logic tests.
* Test **behaviour that must never regress**: data-loss scenarios, error handling, migrations.
* For anything touching files, use `TempDir`.

## Tests that read real data

`tests/test_save.cpp` deliberately loads the shipped `data/` folder (this also validates the JSON files). It therefore assumes:

| Assumption | Used by |
|---|---|
| items `potion_health` (alias `health_potion`), `gold_coin`, `iron_sword` (with `defaultState.durability`), `crystal_shard` (`maxStack` 99), `rusty_key` | creation, round-trip, item-use tests |
| `new_game.json` gives 3 `potion_health` and 25 `gold_coin` | "new game writes starting inventory" |
| worlds `dark_forest`, `crystal_caves` (index 1, name "The Crystal Caves"), `sky_islands` (`minimumLevel` 5), "The Dragon's Peak" | migration and unlock tests |

If you change these on purpose, update the tests in the same commit.

## Fixtures

`tests/fixtures/legacy/saveData*.json` are the **real saves of the first prototype**. They guarantee that version-1 files keep importing correctly. Do not edit them; add new fixtures next to them when a new format version appears.

## Sanitizers

Run the suite under AddressSanitizer and UBSan after changing ownership, lifetimes or containers (command in [Building](building.md#debugging-memory-problems)). This is how a dangling reference in the test macro itself was found.

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Building](building.md) · [Conventions](conventions.md) · [How-to: add a test](how-to.md#add-a-test)
