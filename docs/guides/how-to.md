# How-to recipes

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md#7-where-do-i-put) · Related: [Conventions](conventions.md) · [Data files](../reference/data-files.md)

Step-by-step recipes for the most common changes.

| Recipe | Touches |
|---|---|
| [Add an item](#add-an-item) | `data/items.json` |
| [Rename an item](#rename-an-item) | `data/items.json` |
| [Add an item with changing data](#add-an-item-with-changing-data) | `data/items.json` |
| [Add an item behaviour](#add-an-item-behaviour) | `src/core/gameplay/` |
| [Change the starting kit or inventory size](#change-the-starting-kit-or-inventory-size) | `data/new_game.json` |
| [Add a world](#add-a-world) | `data/worlds.json` |
| [Add a saved player stat](#add-a-saved-player-stat) | `PlayerProfile`, `SaveCodec` |
| [Change the save format](#change-the-save-format) | `SaveCodec`, tests |
| [Add a screen](#add-a-screen) | `src/states/` |
| [Add a constant or an asset](#add-a-constant-or-an-asset) | `src/engine/Config.hpp` |
| [Add a test](#add-a-test) | `tests/` |

---

## Add an item

No C++ needed. Concepts: [Items](../features/items.md).

1. Open `data/items.json` and add an entry to the `items` array:

   ```json
   {
     "id": "bread",
     "name": "Bread",
     "description": "A simple loaf.",
     "category": "consumable",
     "maxStack": 20,
     "value": 3,
     "properties": { "heal": 10 }
   }
   ```
2. The `id` must be unique and stable (saves will refer to it).
3. Because the category is `consumable` and `properties.heal` exists, the built-in behaviour already makes it usable.
4. Start the game; press **F2** in game to receive random items for a quick check.

Schema: [Data files: items.json](../reference/data-files.md#itemsjson).

## Rename an item

Keep the old id as an alias so existing saves keep loading:

```json
{ "id": "healing_potion", "aliases": ["potion_health"], "name": "Healing Potion", "category": "consumable", "maxStack": 20, "properties": { "heal": 25 } }
```

Old saves load with the new id; the next save writes the new id. Also update `data/new_game.json` and any test that uses the old id.

## Add an item with changing data

Example: a sword whose durability decreases.

1. In the definition, give new instances an initial value:

   ```json
   "defaultState": { "durability": 100 }
   ```
2. The system that consumes durability (future combat code) reads and writes `stack.state["durability"]` on the specific stack.
3. Nothing changes in `Inventory`: different states never merge, and `state` is saved automatically.

## Add an item behaviour

To give a family of items a "use" effect. Concepts: [Items: behaviours](../features/items.md#behaviours-using-an-item).

1. In `src/core/gameplay/ItemUse.cpp`, inside the anonymous namespace, write the class:

   ```cpp
   class KeyBehavior : public IItemBehavior {
   public:
       UseResult Use(PlayerProfile&, const ItemDefinition& def, const ItemStack&) const override {
           return {UseOutcome::Kept, "You hold the " + def.name + " up. Nothing happens... yet."};
       }
   };
   ```
2. Register it in `RegisterDefaultBehaviors`:

   ```cpp
   registry.Register("quest", std::make_shared<KeyBehavior>());
   ```
3. Items resolve their behaviour by `behavior` if set, otherwise by `category`. Here every item with `"category": "quest"` is now usable.
4. Return `Consumed` to spend one unit, `Kept` to keep it, `Failed` for "not now", `NotUsable` for no effect.
5. Add a test in `tests/test_save.cpp` (see the existing item-use test).

No change to `Inventory` or `InventoryState`.

## Change the starting kit or inventory size

Edit `data/new_game.json`: `startingItems` (`id`, `qty`), `inventoryCapacity`, `level`. Existing saves are not affected. Update tests that assume the shipped kit ([Testing](testing.md#tests-that-read-real-data)).

## Add a world

1. Add an entry to `data/worlds.json`:

   ```json
   {
     "id": "frozen_marsh",
     "name": "The Frozen Marsh",
     "description": "Ice, fog and things beneath it.",
     "minimumLevel": 7,
     "difficulty": "Hard",
     "background": "media/worlds/frozen_marsh.png"
   }
   ```
2. Order in the file is the order on the selection screen.
3. The lock state is automatic from `minimumLevel` ([Worlds](../features/worlds.md#unlock-rule)).
4. `background` is optional; without it the game shows a plain dark background.

Adding a world does not break existing saves. Never reuse an `id` for a different place.

## Add a saved player stat

Example: `mana`. Concepts: [Player and session](../features/player-and-session.md), [Save system](../features/save-system.md#evolving-the-format).

1. Add the field to `PlayerProfile` with a default (`int mana = 50;`).
2. In `SaveCodec.cpp`: write it in `EncodeSession` (`player["mana"] = p.mana;`) and read it in `DecodeSession` with `GetOr<int>(p, "mana", 50)`, clamping it if it has a range.
3. Because it is an **optional field with a default**, old saves still load: **no version bump**.
4. Add a round-trip assertion to the save/load test in `tests/test_save.cpp`.
5. Show it in `GameplayState::RefreshHud` if relevant.

## Change the save format

Needed only when a field is **renamed, moved, or changes meaning** (adding an optional field does not need this).

1. Increase `kCurrentSaveVersion` in `src/core/save/SaveCodec.hpp`.
2. In `MigrateSaveData` add a step that upgrades the previous version, then sets the new `version`:

   ```cpp
   if (current == 2) {
       // ...rewrite the JSON from layout 2 to layout 3...
       data["version"] = 3;
       current = 3;
   }
   ```
3. Update `EncodeSession` / `DecodeSession` to the new layout.
4. Add a test that feeds a hand-written old-version JSON through `MigrateSaveData` and `DecodeSession` (see the migration tests in `tests/test_save.cpp`). Keep a real old file in `tests/fixtures/` when possible.
5. Update [Save system](../features/save-system.md#versioning-and-migration), [Data files](../reference/data-files.md#save-file) and the version table.

Older games refuse the newer file with `SaveTooNewError`, which is the intended behaviour.

## Add a screen

Concepts: [Screens and UI](../features/screens-and-ui.md).

1. Create `src/states/MyState.hpp` and `.cpp`:

   ```cpp
   #pragma once
   #include "engine/GameContext.hpp"

   namespace Oathbound {
       class MyState : public State {
       public:
           using State::State;                       // inherit State(GameContext&)
           void OnEnter() override;                  // load assets, build sprites
           void HandleEvent(const sf::Event& event) override;
           void Update(float dt) override;
           void Draw() override;
       private:
           sf::Sprite _background;
       };
   }
   ```
2. In `OnEnter`, get assets through `_ctx.assets.Texture(paths::kSomething)`; start a fade with `_ctx.fade.Start(true, config::kFadeTime)` if wanted.
3. In `HandleEvent`, use `input::KeyPressed` / `input::ClickedOn`. Navigate with `_ctx.states.Push(...)`, `Replace(...)` or `Pop()` (see [navigation](../features/screens-and-ui.md#navigation)).
4. Draw with `_ctx.window.draw(...)`.
5. Override `IsOverlay()` to return `true` if the screen should appear over the one below.
6. Open it from another screen: `_ctx.states.Push(std::make_unique<MyState>(_ctx));`
7. Files in `src/states/` are picked up by CMake automatically (re-run `cmake`).
8. Put no game rules in it: call `core` ([UI rules](../features/screens-and-ui.md#ui-rules)).

## Add a constant or an asset

Add it to `src/engine/Config.hpp` (`config::` for tunables, `paths::` for files) as `inline constexpr`, and use it from the screen. Place the file under `media/`.

## Add a test

[Testing: writing a test](testing.md#writing-a-test). Create or extend a `tests/test_*.cpp`, use `TEST_CASE` and `CHECK_EQ`, prefer `MakeTestItems()` and `TempDir`.

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Conventions](conventions.md) · [Items](../features/items.md) · [Save system](../features/save-system.md) · [Screens and UI](../features/screens-and-ui.md) · [Data files](../reference/data-files.md)
