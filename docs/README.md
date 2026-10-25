# Oathbound documentation

Welcome. This folder is the single source of truth for **how Oathbound is built** and **how to extend it**.
Each document is short, focused on one topic, and links to the others.

## Start here

| If you want to... | Read |
|---|---|
| Understand the whole project (layers, folders, data flow) | [Architecture](ARCHITECTURE.md) |
| Build and run the game, fix a build error | [Building](guides/building.md) |
| Add an item, a world, a screen, a save field... | [How-to recipes](guides/how-to.md) |
| Know the coding rules before contributing | [Conventions](guides/conventions.md) |

## Features

One document per feature or system. They describe the design, the rules, the public API, and how to extend it.

| Feature | Summary |
|---|---|
| [Inventory](features/inventory.md) | Slot-based container: add, remove, move, sort, persist. Knows nothing about item types. |
| [Items](features/items.md) | Item definitions vs. item instances, per-instance state, behaviours ("use" actions). |
| [Save system](features/save-system.md) | File format, atomic writes, backups, versioned migration, legacy import. |
| [Player and session](features/player-and-session.md) | `PlayerProfile` and `GameSession`: what the running game holds and when it is saved. |
| [Worlds](features/worlds.md) | World definitions, unlock rules, world selection. |
| [Screens and UI](features/screens-and-ui.md) | The state machine, every screen, controls, how to add a screen. |
| [Engine](features/engine.md) | Main loop, `GameContext`, assets, fade, player avatar, configuration constants. |

## Reference

| Document | Summary |
|---|---|
| [Data files](reference/data-files.md) | JSON schemas of `items.json`, `worlds.json`, `new_game.json` and the save file. |

## Guides

| Guide | Summary |
|---|---|
| [Building](guides/building.md) | Requirements, commands, CMake options, Windows layout, troubleshooting. |
| [Testing](guides/testing.md) | The test framework, what is covered, how to write a test, sanitizers. |
| [Conventions](guides/conventions.md) | Naming, includes, ownership, error handling, review checklist. |
| [How-to recipes](guides/how-to.md) | Step-by-step recipes for the most common changes. |
| [Prototype migration](guides/prototype-migration.md) | What changed from the original prototype, and how old saves are imported. |

## Conventions used in these documents

* File paths are relative to the repository root (for example `src/core/inventory/Inventory.hpp`).
* `Code font` is used for identifiers, files and JSON keys.
* Diagrams are written in [Mermaid](https://mermaid.js.org/), which GitHub and most editors render natively.
* Every document ends with a **Related** section so you can keep navigating.
