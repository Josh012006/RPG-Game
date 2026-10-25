#pragma once

#include "engine/GameContext.hpp"

namespace Oathbound {

    // Worlds come from WorldDatabase; "locked" is derived from the player's level.
    // Choosing a world is stored in the CURRENT session and saved to the CURRENT save file
    // (the old code always wrote to data/saveData.json, whichever save had been loaded).
    class WorldSelectionState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float) override {}
        void Draw() override;

    private:
        void Refresh();
        void Confirm();

        sf::Sprite _background;
        sf::Text _title, _instructions, _name, _description, _details, _message, _counter;
        std::size_t _selected = 0;
    };
}
