#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "Game.hpp"


namespace Josh {
    class SplashState : public State {
    public:
        // We pass the GameDataRef to the constructor to enable the SplashState to have access to the
        // global data of the game
        SplashState(GameDataRef data);

        void Init();

        void HandleInput();
        void Update(float dt);
        void Draw(float dt);

    private:
        // We store the GameDataRef here
        GameDataRef _data;

        // Used to keep track of how long the state has been displayed
        sf::Clock _clock;

        // The background of the state
        sf::Sprite _background;

        // A boolean to keep track of the fade in effect
        bool fadedIn = false;
    };
}
