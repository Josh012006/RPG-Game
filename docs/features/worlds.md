# Worlds

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · Related: [Data files](../reference/data-files.md#worldsjson) · [Screens and UI](screens-and-ui.md#worldselectionstate) · [Player and session](player-and-session.md)

The places the player can enter, and the rule that decides whether they are open.

* Code: `src/core/world/WorldDatabase.*`
* Data: `data/worlds.json` ([schema](../reference/data-files.md#worldsjson))
* Screen: `src/states/WorldSelectionState.*`

## WorldDefinition

Pure data. (The prototype's `World` also contained an `sf::Sprite` and an `sf::FloatRect`; rendering state does not
belong in game data.)

| Field | Type | Default | Meaning |
|---|---|---|---|
| `id` | string | **required** | Stable slug (`"dark_forest"`); this is what saves store in `currentWorldId` |
| `name` | string | **required** | Display name |
| `description` | string | `""` | Shown on the selection screen |
| `difficulty` | string | `"Normal"` | Display text |
| `minimumLevel` | int | `1` | Level required to enter (at least 1) |
| `background` | string | `""` | Optional image path used by `GameplayState` |

## WorldDatabase

| Method | Description |
|---|---|
| `LoadFromFile(path)` / `LoadFromJson(json)` | Throws `std::runtime_error` on invalid data (missing `id`/`name`, duplicate ids) |
| `All()` | Worlds in file order (the order of the selection screen) |
| `Find(id)` | `nullptr` if unknown |
| `FindByName(name)` | Used only to migrate version-1 saves, which stored the display name |
| `IndexOf(id)` | `std::optional<size_t>`; used to pre-select the current world |

## Unlock rule

The "locked" flag is **derived**, never stored:

```cpp
bool IsUnlockedFor(int playerLevel) const { return playerLevel >= minimumLevel; }
```

In the prototype, `locked` was hard-coded in the JSON and `minimumLevel` was ignored, so the two could disagree.
To make a world require something other than level (a quest flag, an item), extend this function; callers
do not change.

## World selection flow

1. `WorldSelectionState` lists `content.worlds.All()` and **pre-selects** the world of `session->player.currentWorldId`.
2. A locked world shows its requirement and cannot be entered ("Locked: reach level N to enter.").
3. Confirming an unlocked world writes `currentWorldId` into the **session**, **saves** it, and replaces the screen with `GameplayState`.

## Unknown world ids

If a save contains a `currentWorldId` that no longer exists, nothing breaks: the id is kept in the save, the selection
screen starts on the first world, and `GameplayState` shows "Unknown world" in the HUD with the fallback background.

## Background images

`GameplayState` draws `background` stretched to the window when the file exists, and a plain dark rectangle otherwise.
Today only `dark_forest` has one (`media/worlds/The_dark_forest.png`;).

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Data files](../reference/data-files.md#worldsjson) · [Screens and UI](screens-and-ui.md) · [Save system](save-system.md) · [How-to: add a world](../guides/how-to.md#add-a-world)
