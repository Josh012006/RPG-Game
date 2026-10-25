# Oathbound

**Oathbound** is an RPG developed in **C++17** with **SFML 2.6.2**, built with CMake
(Windows, Linux, macOS).

## Features

* 2D RPG with exploration and combat *(planned)*
* **Inventory and item management** *(implemented: data-driven items, slots, use, save/load)*
* Dialogues and interactions with NPCs *(planned)*
* Quest system and character progression *(planned)*

## Documentation

Everything about how the project is built lives in [`docs/`](docs/README.md):

| Start with | |
|---|---|
| [Architecture](docs/ARCHITECTURE.md) | Layers, folders, what belongs where, data flow |
| [Building](docs/guides/building.md) | Requirements, commands, troubleshooting |
| [How-to recipes](docs/guides/how-to.md) | Add an item, a world, a screen, a save field... |
| [Conventions](docs/guides/conventions.md) | Coding rules and review checklist |

Features: [Inventory](docs/features/inventory.md) · [Items](docs/features/items.md) · [Save system](docs/features/save-system.md) · [Player and session](docs/features/player-and-session.md) · [Worlds](docs/features/worlds.md) · [Screens and UI](docs/features/screens-and-ui.md) · [Engine](docs/features/engine.md) · [Data files](docs/reference/data-files.md)

## Project layout

```
Oathbound/
├─ src/
│  ├─ core/       Game rules, data and saves. NO SFML: unit-tested
│  ├─ engine/     SFML infrastructure: loop, state machine, assets, fade, avatar
│  ├─ states/     The screens (one class each)
│  └─ main.cpp
├─ tests/         Unit tests of core (ctest)
├─ data/          STATIC game data: items.json, worlds.json, new_game.json
├─ saves/         Player saves (created at runtime, git-ignored)
├─ media/         Images and fonts
├─ third_party/   Bundled headers (nlohmann/json, SFML headers on Windows)
├─ docs/          Documentation
└─ lib/           SFML import libs (Windows only)
```

See [Architecture](docs/ARCHITECTURE.md#4-project-layout) for the full file-by-file layout.

## Build

Requirements: CMake >= 3.16, a C++17 compiler, SFML 2.6.x (`graphics window system`).
On Windows with the bundled SFML, put its headers in `third_party/include/SFML` and its
libs in `lib/` (DLLs next to the executable in `bin/`).

```bash
cmake -S . -B build
cmake --build build --config Release
```

Run **from the project root** (the game also tries to find it on its own):

```bash
./bin/oathbound          # Windows: .\bin\oathbound.exe
```

### Tests

```bash
cmake --build build --config Release --target oathbound_tests
ctest --test-dir build -C Release --output-on-failure
```

Build only the tests, without SFML: `cmake -S . -B build -DOATHBOUND_BUILD_GAME=OFF`.

## Controls

| Where | Keys |
|---|---|
| Menus | Mouse / Arrows, Enter, Esc |
| Gameplay | Arrows / WASD / ZQSD to move, Shift to run, **I** inventory, F5 save, Esc save + menu |
| Inventory | Arrows, **Enter** use, **X** drop one, **Space** pick up / put down, **S** sort, Esc close |
| Debug | **F2** gives a random item (`config::kEnableDebugKeys`) |

## Upgrading from the prototype saves

Old `data/saveData*.json` files are imported automatically at first launch into `saves/`
(then renamed `*.migrated`). See [Prototype migration](docs/guides/prototype-migration.md).

## Contributions

Contributors: **Jeff Vieyra** ([BJeff17](https://github.com/BJeff17)),
**Anis Morat** ([Shiro-qb](https://github.com/Shiro-qb)),
**Gérald Guidi** ([cedric20061](https://github.com/cedric20061)),
**Josué Mongan** ([Josh012006](https://github.com/Josh012006)).
Create a branch, make your changes, and submit a pull request.

## License

**Custom License – Non-Commercial Use with Attribution Requirement**. See [LICENSE](LICENSE).
