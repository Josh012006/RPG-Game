#pragma once

// Replaces DEFINITIONS.hpp: typed constants instead of #define macros (no collisions, no
// surprises with operator precedence, visible to the debugger, namespaced).

namespace Oathbound::config {
    inline constexpr const char* kGameName = "Oathbound";
    inline constexpr unsigned kScreenWidth = 1792;
    inline constexpr unsigned kScreenHeight = 990;

    inline constexpr float kTimeStep = 1.0f / 60.0f;   // fixed update rate
    inline constexpr float kSplashShowTime = 3.5f;     // seconds, counted from the splash start
    inline constexpr float kFadeTime = 1.5f;

    // Developer shortcuts in GameplayState (F2 = give a random item). Turn off for releases.
    inline constexpr bool kEnableDebugKeys = true;
}

// All asset paths in ONE place. They are relative to the game root (see LocateGameRoot in main.cpp).
namespace Oathbound::paths {
    inline constexpr const char* kDataDir = "data";    // static game data (items, worlds...)
    inline constexpr const char* kSaveDir = "saves";   // player saves (gitignored)

    inline constexpr const char* kMainFont = "media/Fonts/mainFont.ttf";

    inline constexpr const char* kGameIcon = "media/gameIcon.png";
    inline constexpr const char* kCursor = "media/cursor.png";

    inline constexpr const char* kPaperTexture = "media/paper_texture.png";
    inline constexpr const char* kBackgroundLayout = "media/backgroundLayout.png";
    inline constexpr const char* kButtonLayout = "media/buttonLayout.png";

    inline constexpr const char* kSplashBackground = "media/StateResources/SplashState/splashBackground.png";
    inline constexpr const char* kMainMenuBackground = "media/StateResources/MainMenuState/background.png";
    inline constexpr const char* kNewGameButton = "media/StateResources/MainMenuState/newGameButton.png";
    inline constexpr const char* kContinueButton = "media/StateResources/MainMenuState/continueGameButton.png";
    inline constexpr const char* kNameWritingSpace = "media/StateResources/NameEntryState/writingSpace.png";

    inline constexpr const char* kHeroSheet = "media/hero.png";
}
