#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "Game.hpp"


namespace Josh {
    class MainMenuState : public State {
    public:

        MainMenuState(GameDataRef data);

        void Init();

        void HandleInput();
        void Update(float dt);
        void Draw(float dt);

    private:
        // We store the GameDataRef here
        GameDataRef _data;

        // The background of the state
        sf::Sprite _background;
        sf::Sprite _title;
        sf::Sprite _newGameButton;
        sf::Sprite _continueGameButton;

    };
}



