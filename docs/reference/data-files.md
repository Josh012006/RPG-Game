# Data files reference

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · Related: [Items](../features/items.md) · [Worlds](../features/worlds.md) · [Save system](../features/save-system.md)

Schemas of every JSON file the game reads or writes. Static content lives in `data/`; the player's progress lives in `saves/`.

| File | Written by | Loaded by | Required |
|---|---|---|---|
| [`data/items.json`](#itemsjson) | designers | `ItemDatabase` | yes |
| [`data/worlds.json`](#worldsjson) | designers | `WorldDatabase` | yes |
| [`data/new_game.json`](#new_gamejson) | designers | `GameContent` | no (defaults apply) |
| [`saves/save_N.json`](#save-file) | the game | `SaveManager` | created at run time |

If a required file is missing or invalid, start-up fails with `Fatal error: <file>: <reason>`.

## items.json

```json
{ "items": [ { "id": "...", ... } ] }
```

Top level: an object with an `items` array. Each entry:

| Key | Type | Required | Default | Notes |
|---|---|---|---|---|
| `id` | string | **yes** | | Unique, stable, non-empty. Lower snake_case by convention (`potion_health`) |
| `name` | string | no | `id` | Display name |
| `description` | string | no | `""` | |
| `category` | string | no | `"misc"` | Free text. Also the default behaviour key |
| `behavior` | string | no | `""` | Behaviour key; empty means "use `category`" |
| `maxStack` | integer | no | `1` | Must be >= 1 |
| `value` | integer | no | `0` | |
| `aliases` | string[] | no | `[]` | Former ids. Must not clash with any id or other alias |
| `tags` | string[] | no | `[]` | Free labels |
| `properties` | object | no | `{}` | Static, type-specific data (`{"heal": 25}`, `{"damage": 7}`) |
| `defaultState` | object | no | none | Initial per-instance `state` of new stacks (`{"durability": 100}`) |

Validation errors name the entry, e.g. `data/items.json: items[3]: item 'sword': maxStack must be >= 1`.
Duplicate ids and alias conflicts are rejected.

Example:

```json
{
  "id": "iron_sword",
  "name": "Iron Sword",
  "description": "A plain but reliable blade. Wears down with use.",
  "category": "weapon",
  "maxStack": 1,
  "value": 60,
  "tags": ["melee", "one_handed"],
  "properties": { "damage": 7, "slot": "main_hand" },
  "defaultState": { "durability": 100 }
}
```

Currently shipped categories: `currency`, `consumable`, `weapon`, `armor`, `material`, `quest`. Only `consumable` has a built-in behaviour
([Items](../features/items.md#built-in-behaviour)). Concepts: [Items](../features/items.md).

## worlds.json

```json
{ "worlds": [ { "id": "dark_forest", "name": "The Dark Forest", ... } ] }
```

| Key | Type | Required | Default | Notes |
|---|---|---|---|---|
| `id` | string | **yes** | | Unique, stable slug. Stored in saves as `currentWorldId` |
| `name` | string | **yes** | | Display name |
| `description` | string | no | `""` | |
| `difficulty` | string | no | `"Normal"` | Display text |
| `minimumLevel` | integer | no | `1` | Values below 1 become 1. Drives the unlock rule |
| `background` | string | no | `""` | Image path, relative to the game root |

Order in the file is the order on the selection screen. The old `locked` and numeric `id` fields of the prototype are gone:
locking is [derived from the player's level](../features/worlds.md#unlock-rule). Concepts: [Worlds](../features/worlds.md).

## new_game.json

Optional. Describes what a brand-new character starts with.

```json
{
  "level": 1,
  "inventoryCapacity": 24,
  "startingItems": [
    { "id": "iron_sword", "qty": 1 },
    { "id": "potion_health", "qty": 3 },
    { "id": "gold_coin", "qty": 25 }
  ]
}
```

| Key | Type | Default | Notes |
|---|---|---|---|
| `level` | integer | `1` | At least 1 |
| `inventoryCapacity` | integer | `24` | At least 1; the inventory clamps it to 4096 |
| `startingItems[]` | `{ "id": string, "qty": integer }` | none | `qty` defaults to 1. An unknown `id` is ignored with a warning. Items are added with `Inventory::Add(id, qty)`, so they receive their `defaultState` |

`inventoryCapacity` is also the capacity used when a save does not state its own.

## Save file

Written by the game, never by hand. Full explanation: [Save system](../features/save-system.md).

| Path | Type | Default if absent when loading | Notes |
|---|---|---|---|
| `version` | integer | `1` | Current is **2**. Greater than the game's version is refused |
| `meta.createdAt` | string | `""` | ISO-8601 UTC |
| `meta.updatedAt` | string | `""` | ISO-8601 UTC; refreshed on every save |
| `meta.playTimeSeconds` | number | `0` | |
| `player.name` | string | **required** | Missing or empty makes the save unusable |
| `player.level` | integer | `1` | At least 1 |
| `player.maxHealth` | integer | `100` | At least 1 |
| `player.health` | integer | `maxHealth` | Clamped to `0..maxHealth` |
| `player.currentWorldId` | string | `""` | World id; may refer to a world that no longer exists |
| `player.inventory.capacity` | integer | game default | Clamped to 4096 |
| `player.inventory.slots[]` | objects | empty | `{ "slot": int, "id": string, "qty": int >= 1, "state"?: object }` |

Version-1 files (from the prototype) look different (`player.currentWorld` held a display name). They are migrated automatically:
[Save system: versioning](../features/save-system.md#versioning-and-migration).

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Items](../features/items.md) · [Inventory](../features/inventory.md) · [Worlds](../features/worlds.md) · [Save system](../features/save-system.md) · [How-to](../guides/how-to.md)
