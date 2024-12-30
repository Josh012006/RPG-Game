#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include "AssetManager.hpp"
#include "InputManager.hpp"
#include "Window.hpp"


namespace Josh {

    // A structure keeping all the main features of our game
    struct GameData {
        Josh::StateMachine machine;
        // sf::RenderWindow window;
        Josh::Window window;
        Josh::AssetManager assets;
        Josh::InputManager input;
    };

    typedef std::shared_ptr<GameData> GameDataRef;


    class Game {
    public:
        Game(int width, int height, std::string title);

    private:
        // The framerate
        const float dt = 1.0f / 60.0f;
        // The clock that manage time
        sf::Clock _clock;

        // Stores our game's data
        Josh::GameDataRef _data = std::make_shared<Josh::GameData>();


        // Managing the running of the game
        void Run();
    };
}






