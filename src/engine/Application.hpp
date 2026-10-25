#pragma once

#include <SFML/Graphics.hpp>

#include "engine/GameContext.hpp"

namespace Oathbound {

    // Owns the GameContext and runs the main loop. Replaces `Game`, whose constructor ran the
    // whole game (so `main` had to create a temporary object to start it).
    class Application {
    public:
        // Loads the game data, opens the window and queues the first state.
        // Throws std::runtime_error with a readable message if something essential is missing.
        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        int Run();

    private:
        void SetupWindowDecorations();  // icon + custom cursor (cosmetic: failures only warn)
        void SaveOnExit();

        GameContext _ctx;
        sf::Cursor _cursor;  // must stay alive as long as the window uses it
    };
}
