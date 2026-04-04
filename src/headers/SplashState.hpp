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

        // The background of the state
        sf::Sprite _background;
    };
}
