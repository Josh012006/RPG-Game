# Conventions

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · Related: [How-to](how-to.md) · [Testing](testing.md)

The rules the code already follows. New code should look like old code.

## Contents

1. [Language and style](#language-and-style)
2. [Naming](#naming)
3. [Files and includes](#files-and-includes)
4. [Ownership and lifetime](#ownership-and-lifetime)
5. [Error handling](#error-handling)
6. [Working with JSON](#working-with-json)
7. [Dependency rules](#dependency-rules)
8. [Review checklist](#review-checklist)

---

## Language and style

* **C++17.** `core` compiles warning-free with `-Wall -Wextra -Wpedantic`; keep it that way.
* Code, comments, identifiers and documentation are in **English**.
* 4-space indentation, no tabs. Opening brace on the same line. Everything inside `namespace Oathbound { ... }` is indented one level.
* Prefer `const` everywhere it is possible; mark methods `const` when they do not modify the object.
* Mark single-argument constructors `explicit`, overrides `override`, results that must be read `[[nodiscard]]`.
* Initialise members at their declaration (`int level = 1;`) rather than in constructors when a simple default exists.
* Prefer `static_cast` to C casts; `nullptr` to `NULL`; `enum class` to `enum`; `constexpr` to `#define`.
* Comments explain **why**, not what. A comment on a non-obvious invariant or ordering requirement is mandatory (see `GameContext`).

## Naming

| Thing | Style | Example |
|---|---|---|
| Types, functions, methods | `PascalCase` | `Inventory`, `ApplyPending`, `RemoveFromSlot` |
| Interfaces | `I` + `PascalCase` | `IItemCatalog`, `IItemBehavior` |
| Data members of classes | `_camelCase` | `_slots`, `_catalog` |
| Public members of plain data structs | `camelCase` | `maxStack`, `currentWorldId` |
| Constants | `kPascalCase` | `kMaxCapacity`, `kScreenWidth` |
| Local variables, parameters | `camelCase` | `remaining`, `playerLevel` |
| Namespaces | `Oathbound`, plus lower-case sub-namespaces | `config`, `paths`, `log`, `ui`, `input`, `player_name` |
| Files | the class name, `.hpp` / `.cpp` | `ItemDatabase.hpp` |
| JSON keys | `camelCase` | `maxStack`, `minimumLevel` |
| Item / world / save ids | `lower_snake_case` | `potion_health`, `dark_forest`, `save_1` |

`struct` is for plain data without invariants (`ItemDefinition`, `SaveSummary`); `class` is for objects that protect their state (`Inventory`, `StateMachine`).

## Files and includes

* Headers use `#pragma once`.
* Include project headers from the `src/` root: `#include "core/inventory/Inventory.hpp"`. Standard and third-party headers use angle brackets.
* **Include what you use**; do not rely on another header pulling it in.
* **Never `using namespace` in a header.**
* One class per file pair, named after the class. Small tightly related declarations may share a header (`PlayerProfile` and `player_name::`).
* Templates and small `inline` functions live in the header; everything else in the `.cpp`.
* Helpers private to a `.cpp` go in an anonymous `namespace { }`.
* Asset paths and tunable constants go in `src/engine/Config.hpp`, not as literals.

## Ownership and lifetime

1. **No `new` / `delete`.** Use values, containers, `std::make_unique`.
2. `std::unique_ptr` for single ownership; `std::shared_ptr` only when ownership is genuinely shared (today only `ItemBehaviorRegistry`).
3. A **raw pointer or reference does not own**. Before holding one, ask: *what outlives what?*
4. Use a **pointer** (not a reference) as a class member when the class must stay copyable/assignable (`Inventory::_catalog`); use a **reference** when the target is guaranteed for the object's whole life and the object is non-copyable (`State::_ctx`).
5. Anything other objects point to (textures, fonts, item definitions) must have a **stable address**: heap-allocated in a map, or a `std::deque`. A `std::vector` may relocate its elements when it grows.
6. **Member order is destruction order** (reverse of declaration). Document any order that matters, as in `GameContext`.
7. Pass by `const T&` to read, `T&` to modify, by value for small types and for parameters you will store (then `std::move` them). Use a pointer for an *optional* output.
8. Do not use `auto&&` on an arbitrary expression to keep a sub-object of a temporary; copy the small value instead.

## Error handling

| Situation | Do |
|---|---|
| Normal absence (empty slot, unknown key) | return `nullptr`, `std::optional`, or a default; do **not** throw |
| Operation that can really fail (file, invalid data) | `throw` a `std::runtime_error` (or `SaveError`) with a message that names the file/entry |
| Catching | by `const&`, **most specific first** (`SaveTooNewError` before `SaveError` before `std::exception`) |
| In a screen | catch at the boundary and **display** the message |
| In `core` | never print to the player; throw, log with `log::Warn/Error` |
| Loading player data | be **tolerant**: warn and keep, do not discard ([Inventory](../features/inventory.md#loading-rules-loadjson)) |
| Mutating data | be **strict**: refuse invalid input |

## Working with JSON

* Check before reading: `find`, `contains`, `is_object()`, `is_number_integer()`... `j["key"]` on a constant or non-object value throws or asserts.
* Read optional values through a helper that returns a default on a missing key or wrong type (`GetOr<T>` in `SaveCodec.cpp`, `ItemDefinition::Property<T>`).
* Reading a number of unknown sign: use `long long`, validate, then convert to `size_t`.
* Never trust a loaded file: validate ranges (`std::clamp`, `max`).
* Write files only through `WriteJsonFileAtomic`.

## Dependency rules

* `core` never includes SFML or anything from `engine/` or `states/`.
* `engine` never includes `states/` (except `Application` pushing the first screen).
* A screen never reads or writes files or JSON directly; it calls `SaveManager` / `GameContent`.
* Game rules go in `core/gameplay`, never in a screen.
* New static content goes in `data/`; new behaviour goes in code. Do not encode content in C++.

Quick check: `grep -r "SFML" src/core` must return nothing.

## Review checklist

- [ ] The change is in the right folder ([Architecture: folder rules](../ARCHITECTURE.md#5-folder-rules-what-belongs-where)).
- [ ] No SFML include in `src/core`.
- [ ] Errors are thrown in `core` and displayed in screens.
- [ ] Loading code is tolerant; mutating code is strict.
- [ ] Any new saved field has a default on load; any breaking change bumps the save version and adds a migration ([How-to](how-to.md#change-the-save-format)).
- [ ] Ids are strings, not indexes.
- [ ] A test covers new `core` behaviour ([Testing](testing.md)).
- [ ] `core` still compiles with `-Wall -Wextra -Wpedantic` and the tests pass.
- [ ] Documentation updated if a public API, data schema or flow changed.

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [How-to](how-to.md) · [Testing](testing.md) · [Building](building.md)
