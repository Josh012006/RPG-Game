#pragma once

#include <string>

#include "engine/GameContext.hpp"

namespace Oathbound {

    // Was NameEntryState. It no longer writes JSON itself: it asks the SaveManager to create
    // the character, which also gives them their starting inventory.
    class CharacterCreationState : public State {
    public:
        using State::State;

        void OnEnter() override;
        void HandleEvent(const sf::Event& event) override;
        void Update(float) override {}
        void Draw() override;

    private:
        void TryStart();
        void ShowError(const std::string& message);

        sf::Sprite _background;
        sf::Sprite _layout;
        sf::Sprite _startButton;
        sf::Sprite _nameField;

        sf::Text _startText;
        sf::Text _instructions;
        sf::Text _nameText;
        sf::Text _errorText;

        std::string _name;
        bool _started = false;  // guards against a double click / double Enter creating two saves
    };
}
