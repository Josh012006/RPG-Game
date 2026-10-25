# Engine

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · Related: [Screens and UI](screens-and-ui.md) · [Building](../guides/building.md)

The SFML infrastructure that every screen relies on. It contains no game rules.

* Code: `src/engine/*`, `src/main.cpp`

## Contents

1. [Start-up](#start-up)
2. [Main loop](#main-loop)
3. [GameContext](#gamecontext)
4. [AssetManager](#assetmanager)
5. [ScreenFade](#screenfade)
6. [PlayerAvatar](#playeravatar)
7. [Configuration](#configuration)

---

## Start-up

`main.cpp`:

1. `LocateGameRoot(argv[0])` finds the folder that contains `data/items.json` and makes it the working directory. It tries, in order: the current directory, the executable's folder, its parent, its grandparent. The game therefore starts correctly from the root, from `bin/`, or by double-clicking.
2. `Application app; return app.Run();` inside a `try` that prints `Fatal error: ...` and returns a non-zero code.

`Application::Application()`:

1. `GameContent::LoadFromDirectory("data")` (throws with the file name if a data file is invalid).
2. Creates the `SaveManager` on `saves/`.
3. `ImportLegacySaves("data")` ([Save system](save-system.md#legacy-import)).
4. Opens the window (`1792 x 990`, close button, not resizable), sets a 60 FPS limit.
5. Loads the window icon and the custom cursor (failures only warn).
6. Pushes `SplashState`.

## Main loop

`Application::Run()`:

```text
while the window is open:
    states.ApplyPending()                  apply queued Push / Replace / Pop
    accumulator += min(frameTime, 0.25 s)
    for each pending event:
        Closed  -> close the window
        else    -> states.Active().HandleEvent(event)
    while accumulator >= 1/60 s:
        states.Active().Update(1/60)
        fade.Update(1/60)
        session->playTimeSeconds += 1/60   (if a session exists)
        accumulator -= 1/60
    clear, states.DrawAll(), fade.Draw(), display
SaveOnExit()
```

Why a fixed step: the game behaves identically on any machine, and **all speeds are expressed in units per
second** and multiplied by the step. The `min(..., 0.25)` cap stops a long stall (window drag, breakpoint) from
causing a burst of hundreds of updates. `setFramerateLimit(60)` keeps the loop from consuming a full CPU core.

`StateMachine::DrawAll()` draws the top screen, preceded by the screens below it as long as the ones above are
overlays (`IsOverlay()`).

## GameContext

`src/engine/GameContext.hpp`: everything a screen can use, owned by `Application`, passed by reference.

```cpp
struct GameContext {
    sf::RenderWindow window;
    ScreenFade fade;
    AssetManager assets;                    // destroyed AFTER the screens

    GameContent content;                    // static game data
    std::unique_ptr<SaveManager> saves;
    std::unique_ptr<GameSession> session;   // null in the menus

    StateMachine states;                    // KEEP LAST: destroyed first
};
```

**Do not reorder these members.** Members are destroyed in reverse order; screens hold sprites and texts that
point into `assets`, so `states` must be destroyed before `assets`. Reordering would crash on exit.

It replaced the prototype's `shared_ptr<GameData>`: the context outlives every screen, so shared ownership
was unnecessary.

## AssetManager

`AssetManager::Texture(path)` and `Font(path)` load a file on first use and cache it **by path** (there is no second
naming scheme to keep in sync). Properties:

* Returns a **const reference**; callers never copy a texture (copying is a GPU copy).
* Resources are heap-allocated in a `std::unique_ptr` map, so references stay valid for the whole run. Sprites and texts may safely keep pointing at them.
* A missing file throws `std::runtime_error("cannot load texture '<path>'")`, so a bad path is reported immediately with its name.

## ScreenFade

`Start(fadeIn, seconds, colour)` then `Update(dt)` / `Draw(target)`.

* `fadeIn = true`: covered -> visible. `fadeIn = false`: visible -> covered.
* Time based (`alpha += +/- 255 / duration * dt`), so the real duration matches the requested one.
* After a fade **out** the screen **stays covered** until the next `Start`; the next screen is expected to fade in.
* Starting a fade while another runs continues from the current opacity (no visual jump).

## PlayerAvatar

The visible, animated hero. (It replaced `CharacterManager`; the player's *data* is `PlayerProfile` in core.)

* **Sprite sheet:** `media/hero.png`, 576 x 384, holds 8 characters. Each is a block of 3 columns (animation frames) by 4 rows (directions: down, left, right, up) of 48 x 48 pixels. For character `n` (1-8): block origin `x0 = ((n-1) mod 4) * 3 * w`, `y0 = floor((n-1)/4) * 4 * h`.
* **Movement:** the input vector is normalised (diagonals are not faster), then `position += direction * speed * dt`. Speeds: **140 px/s** walking, **260 px/s** running. Position is clamped to the window.
* **Animation:** column = `floor(time * 8) mod 3` while moving; the middle column (1) when standing.
* It points at a texture owned by `AssetManager` and never copies it.

Configure with `AvatarSpec { texturePath, frameWidth, frameHeight, character }`.

## Configuration

`src/engine/Config.hpp` replaces the prototype's `DEFINITIONS.hpp` (macros) with typed constants.

| Namespace | Contents |
|---|---|
| `config::` | `kGameName`, `kScreenWidth` (1792), `kScreenHeight` (990), `kTimeStep` (1/60), `kSplashShowTime` (3.5), `kFadeTime` (1.5), `kEnableDebugKeys` |
| `paths::` | `kDataDir` (`data`), `kSaveDir` (`saves`), and every asset path (`kMainFont`, `kPaperTexture`, `kHeroSheet`...) |

Add a new asset path or constant **here**, not as a literal in a screen. Turn `kEnableDebugKeys` off for releases.

## Input helpers

`src/engine/Input.hpp` turns events into intent (`KeyPressed`, `LeftClick`, `ClickedOn(event, sprite)`). The window is not
resizable and uses the default view, so window pixels equal world coordinates; if resizing or a camera is added,
`ClickedOn` must convert coordinates with `mapPixelToCoords`.

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Screens and UI](screens-and-ui.md) · [Player and session](player-and-session.md) · [Save system](save-system.md) · [Building](../guides/building.md)
