#pragma once

#include <vector>

#include "core/save/SaveManager.hpp"
#include "engine/GameContext.hpp"

namespace Oathbound {

    // Was SaveSelectionState. Shows ALL saves as a list (the old one showed a single entry at a
    // time) and hands the loaded GameSession to the rest of the game through the context.
    class LoadGameState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float) override {}
        void Draw() override;

    private:
        static constexpr int kVisibleRows = 10;

        void Rebuild();
        void LoadSelected();

        sf::Sprite _background;
        sf::Text _title;
        sf::Text _instructions;
        sf::Text _emptyText;
        sf::Text _statusText;
        std::vector<sf::Text> _rows;      // only the visible rows

        std::vector<SaveSummary> _saves;
        int _selected = 0;
        int _top = 0;                     // index of the first visible save
    };
}
