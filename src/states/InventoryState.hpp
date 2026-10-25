#pragma once

#include <optional>
#include <string>

#include "engine/GameContext.hpp"

namespace Oathbound {

    // Inventory screen, drawn over the gameplay. It only talks to Inventory and UseItem():
    // it never needs to know what kinds of items exist; names, descriptions and categories
    // come from the item definitions, and the "state" is shown as raw data.
    //   Arrows: move cursor   Enter: use   X: drop one   Space: pick up / put down (swap, merge)
    //   S: sort               Esc / I: close (saves)
    class InventoryState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void OnExit() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float) override {}
        void Draw() override;
        bool IsOverlay() const override { return true; }

    private:
        static constexpr std::size_t kColumns = 6;
        static constexpr std::size_t kVisibleRows = 6;
        static constexpr float kSlotSize = 96.0f;
        static constexpr float kGap = 10.0f;
        static constexpr float kGridX = 136.0f;
        static constexpr float kGridY = 190.0f;

        Inventory& Inv() { return _ctx.session->player.inventory; }
        void MoveCursor(int dx, int dy);
        void Say(const std::string& message, sf::Color color);

        std::size_t _cursor = 0;
        std::size_t _firstRow = 0;
        std::optional<std::size_t> _grabbed;
        std::string _message;
        sf::Color _messageColor = sf::Color::Black;
    };
}
