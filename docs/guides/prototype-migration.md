# Prototype migration

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · Related: [Save system](../features/save-system.md#legacy-import) · [Building](building.md#troubleshooting)

What changed between the original prototype (namespace `Josh`) and the current architecture, and how old data is brought forward.

## Contents

1. [Upgrading an existing working copy](#upgrading-an-existing-working-copy)
2. [Old saves](#old-saves)
3. [What moved or was renamed](#what-moved-or-was-renamed)
4. [Bugs fixed by the redesign](#bugs-fixed-by-the-redesign)

---

## Upgrading an existing working copy

If you copied the new project over an old checkout, files that exist only in the old tree stay behind and are still compiled (CMake collects every `.cpp` in `src/engine` and `src/states`). Typical symptom: `LNK2019` errors mentioning `Josh::AssetManager` or `NameEntryState.obj`.

1. Find leftovers (they all use `namespace Josh`):

   ```powershell
   Get-ChildItem -Recurse src, include -Include *.cpp,*.hpp | Select-String "Josh" -List | Select-Object -ExpandProperty Path
   ```
   ```bash
   grep -rl "Josh" src include
   ```
2. Delete those files, and the old `include/Oathbound/` folder.
3. Move the vendored headers (`include/SFML`, `include/nlohmann`) to `third_party/include/`.
4. Delete `build/` and reconfigure ([Building](building.md#commands)).

## Old saves

The prototype stored saves as `data/saveData.json` and `data/saveData_N.json` (**version 1**, no `version` field, the world stored by display name).

At the next start they are imported automatically:

| Step | Result |
|---|---|
| Read and migrate | `player.currentWorld` ("The Crystal Caves") becomes `player.currentWorldId` (`crystal_caves`) |
| Write | A new `saves/save_N.json` per file; they get an empty inventory and default health |
| Rename | The original becomes `saveData*.json.migrated`, so it is imported only once |

A file that cannot be read is skipped with a warning and left untouched. The orphan `data/saveData.json` (written by a prototype bug) is imported too, as one more save; it can be removed with `SaveManager::Delete` or by deleting its file in `saves/` (there is no delete button in the UI yet).
Real prototype files are kept in `tests/fixtures/legacy/` as a regression test.

## What moved or was renamed

| Prototype | Now |
|---|---|
| `namespace Josh` | `namespace Oathbound` |
| `Game` (its constructor ran the loop) | `Application` with an explicit `Run()` |
| `GameData` in a `shared_ptr` | `GameContext&` ([Engine](../features/engine.md#gamecontext)) |
| `NameEntryState` | `CharacterCreationState` |
| `SaveSelectionState` | `LoadGameState` |
| `CharacterManager` | `PlayerAvatar` (drawing only; player data is `PlayerProfile`) |
| `Window : sf::RenderWindow` with fade code | plain `sf::RenderWindow` + `ScreenFade` |
| `InputManager` (mouse polling) | `engine/Input.hpp` (event helpers) |
| `DEFINITIONS.hpp` (`#define`) | `engine/Config.hpp` (`constexpr`) |
| `World` struct holding an `sf::Sprite` | `WorldDefinition` (pure data) |
| `include/` (code + SFML + nlohmann) | `src/` for code, `third_party/include/` for dependencies |
| `data/` (content and saves mixed) | `data/` content only, `saves/` for progress |
| `scratch.cpp` (entirely commented out) | removed |
| executable `main` | executable `oathbound` |

## Bugs fixed by the redesign

| Prototype behaviour | Now |
|---|---|
| Choosing a world always wrote to `data/saveData.json`, whichever save was loaded, so the choice was never stored in the right save | The world is stored in the current session and saved to its own file |
| A loaded save was parsed and thrown away | It becomes a `GameSession` shared by all screens ([Player and session](../features/player-and-session.md)) |
| Esc on the Load screen quit the game (the menu had been *replaced*, so the stack emptied) | Sub-screens are *pushed*; Esc pops back to the menu |
| `StateMachine` flags were never initialised explicitly and only one change could be pending | A queue of commands applied at the start of the next frame |
| `CharacterManager` held a **copy** of `AssetManager`; its sprite pointed into that copy | One `AssetManager`, heap-allocated resources with stable addresses |
| A held mouse button re-triggered a click every frame | Actions use `MouseButtonPressed` events |
| Backspace in the name field returned early and dropped the remaining events | Events are handled one by one |
| The loop ran with no frame limit (100% of a CPU core) | 60 FPS limit plus a fixed time step |
| Fade assumed 60 FPS and truncated its speed to an integer | Time-based `ScreenFade` |
| Movement of 0.03 px per call (speed depended on loop frequency) | Pixels per second times the time step |
| `locked` hard-coded in `worlds.json`, `minimumLevel` ignored | Lock derived from the player's level |
| Control characters accepted in names | `player_name::Normalize` / `IsValid` |
| Double click / double Enter created two saves | `_started` guard |
| A missing asset caused an obscure `std::out_of_range` later | A clear exception naming the path |

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Save system](../features/save-system.md) · [Building](building.md) · [Engine](../features/engine.md)
