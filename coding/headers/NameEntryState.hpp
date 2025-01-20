#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "Game.hpp"


namespace Josh {
    class NameEntryState : public State {
    public:

        NameEntryState(GameDataRef data);

        void Init();

        void HandleInput();
        void Update(float dt);
        void Draw(float dt);

    private:
        // We store the GameDataRef here
        GameDataRef _data;

        // The visual components
        sf::Sprite _background;
        sf::Sprite _startGameButton;

        // The font
        sf::Font _mainFont;

    };
}



