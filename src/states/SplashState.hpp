#pragma once

#include "engine/GameContext.hpp"

namespace Oathbound {

    class SplashState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float dt) override;
        void Draw() override;

    private:
        void BeginLeaving();

        sf::Sprite _background;
        float _elapsed = 0.0f;
        bool _leaving = false;
        bool _done = false;
    };
}
