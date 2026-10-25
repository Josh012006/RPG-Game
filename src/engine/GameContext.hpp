#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "core/content/GameContent.hpp"
#include "core/save/GameSession.hpp"
#include "core/save/SaveManager.hpp"
#include "engine/AssetManager.hpp"
#include "engine/ScreenFade.hpp"
#include "engine/StateMachine.hpp"

namespace Oathbound {

    // Everything states need, owned by Application and passed to each state as a reference.
    // (Replaces GameData + shared_ptr: the context outlives every state, so shared ownership
    // was not needed.)
    //
    // Member order matters: members are destroyed in reverse order, and `states` must go first
    // because states hold sprites and texts that point into `assets`.
    struct GameContext {
        sf::RenderWindow window;
        ScreenFade fade;
        AssetManager assets;

        GameContent content;                    // static game data (items, worlds...)
        std::unique_ptr<SaveManager> saves;
        std::unique_ptr<GameSession> session;   // the running game; null in the menus

        StateMachine states;                    // keep last
    };
}
