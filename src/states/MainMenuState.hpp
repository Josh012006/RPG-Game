#pragma once

#include "engine/GameContext.hpp"

namespace Oathbound {

    class MainMenuState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void OnResume() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float) override {}
        void Draw() override;

    private:
        sf::Sprite _background;
        sf::Sprite _newGameButton;
        sf::Sprite _continueButton;
    };
}
