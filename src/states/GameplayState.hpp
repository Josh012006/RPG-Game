#pragma once

#include <random>
#include <string>

#include "engine/GameContext.hpp"
#include "engine/PlayerAvatar.hpp"

namespace Oathbound {

    // Minimal playable shell: a world background, the hero walking around, a HUD, and the
    // hooks to the inventory. Combat, NPCs, quests... plug in here later.
    //   Arrows / WASD / ZQSD : move     Shift : run     I : inventory
    //   F5 : save                       Esc : save and return to the menu
    //   F2 : (debug) give a random item
    class GameplayState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float dt) override;
        void Draw() override;

    private:
        void Save(bool announce);
        void Toast(const std::string& message, sf::Color color);
        void RefreshHud();

        PlayerAvatar _avatar;
        sf::Sprite _background;
        bool _hasBackground = false;
        sf::RectangleShape _fallbackBackground;

        sf::Text _hud, _hint, _toast;
        float _toastTimer = 0.0f;
        std::mt19937 _rng{std::random_device{}()};
    };
}
