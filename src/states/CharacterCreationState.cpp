#include "states/CharacterCreationState.hpp"

#include "engine/Config.hpp"
#include "engine/Input.hpp"
#include "engine/UiHelpers.hpp"
#include "states/WorldSelectionState.hpp"

namespace Oathbound {

    void CharacterCreationState::OnEnter() {
        _background.setTexture(_ctx.assets.Texture(paths::kPaperTexture));
        _layout.setTexture(_ctx.assets.Texture(paths::kBackgroundLayout));
        _startButton.setTexture(_ctx.assets.Texture(paths::kButtonLayout));
        _nameField.setTexture(_ctx.assets.Texture(paths::kNameWritingSpace));

        const float cx = config::kScreenWidth / 2.0f;
        const float cy = config::kScreenHeight / 2.0f;
        const sf::FloatRect bb = _startButton.getGlobalBounds();
        const sf::FloatRect nb = _nameField.getGlobalBounds();

        _startButton.setPosition(cx - bb.width / 2.0f + 450.0f, cy - bb.height / 2.0f + 350.0f);
        _nameField.setPosition(cx - nb.width / 2.0f, 300.0f);

        const sf::Font& font = _ctx.assets.Font(paths::kMainFont);
        _startText = ui::MakeText(font, "Start game", 25, sf::Color::White,
                                  {cx - bb.width / 2.0f + 520.0f, cy - bb.height / 2.0f + 365.0f});
        _instructions = ui::MakeText(font, "Please enter your character's name. Use your keyboard.", 30, ui::kInk,
                                     {cx - 370.0f, 170.0f});
        _nameText = ui::MakeText(font, "", 25, sf::Color::White, {cx - nb.width / 2.0f + 85.0f, 327.0f});
        _errorText = ui::MakeText(font, "", 22, ui::kError, {cx - 370.0f, 235.0f});
    }

    void CharacterCreationState::ShowError(const std::string& message) {
        _errorText.setString(message);
    }

    void CharacterCreationState::HandleEvent(const sf::Event& event) {
        switch (event.type) {
            case sf::Event::TextEntered: {
                const sf::Uint32 c = event.text.unicode;
                if (c == 8) {                                   // backspace
                    if (!_name.empty()) _name.pop_back();
                } else if (c >= 32 && c < 127 && _name.size() < player_name::kMaxLength) {
                    _name.push_back(static_cast<char>(c));     // printable ASCII only (font limitation)
                }
                _nameText.setString(_name);
                _errorText.setString("");
                break;
            }
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Return) TryStart();
                else if (event.key.code == sf::Keyboard::Escape) _ctx.states.Pop();
                break;
            case sf::Event::MouseButtonPressed:
                if (input::ClickedOn(event, _startButton)) TryStart();
                break;
            default:
                break;
        }
    }

    void CharacterCreationState::TryStart() {
        if (_started) return;

        const std::string name = player_name::Normalize(_name);
        if (!player_name::IsValid(name)) {
            ShowError("Please enter a name (1-" + std::to_string(player_name::kMaxLength) + " characters).");
            return;
        }

        try {
            _ctx.session = std::make_unique<GameSession>(_ctx.saves->CreateNew(name));
        } catch (const std::exception& e) {
            ShowError(std::string("Could not create the save: ") + e.what());
            return;
        }

        _started = true;
        _ctx.states.Replace(std::make_unique<WorldSelectionState>(_ctx));
    }

    void CharacterCreationState::Draw() {
        _ctx.window.draw(_background);
        _ctx.window.draw(_layout);
        _ctx.window.draw(_startButton);
        _ctx.window.draw(_startText);
        _ctx.window.draw(_instructions);
        _ctx.window.draw(_nameField);
        _ctx.window.draw(_nameText);
        _ctx.window.draw(_errorText);
    }
}
