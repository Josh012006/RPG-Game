#include "states/MainMenuState.hpp"

#include "engine/Config.hpp"
#include "engine/Input.hpp"
#include "states/CharacterCreationState.hpp"
#include "states/LoadGameState.hpp"

namespace Oathbound {

    void MainMenuState::OnEnter() {
        _background.setTexture(_ctx.assets.Texture(paths::kMainMenuBackground));
        _newGameButton.setTexture(_ctx.assets.Texture(paths::kNewGameButton));
        _continueButton.setTexture(_ctx.assets.Texture(paths::kContinueButton));

        const float cx = config::kScreenWidth / 2.0f;
        const float cy = config::kScreenHeight / 2.0f;
        const sf::FloatRect nb = _newGameButton.getGlobalBounds();
        const sf::FloatRect cb = _continueButton.getGlobalBounds();
        _newGameButton.setPosition(cx - nb.width / 2.0f, cy - nb.height / 2.0f);
        _continueButton.setPosition(cx - cb.width / 2.0f, cy - cb.height / 2.0f + 150.0f);

        _ctx.fade.Start(true, config::kFadeTime);
    }

    void MainMenuState::OnResume() {
        // Back from a game or a sub-screen: no character is active any more.
        _ctx.session.reset();
    }

    void MainMenuState::HandleEvent(const sf::Event& event) {
        if (input::ClickedOn(event, _newGameButton)) {
            _ctx.states.Push(std::make_unique<CharacterCreationState>(_ctx));
        } else if (input::ClickedOn(event, _continueButton)) {
            _ctx.states.Push(std::make_unique<LoadGameState>(_ctx));
        }
    }

    void MainMenuState::Draw() {
        _ctx.window.draw(_background);
        _ctx.window.draw(_newGameButton);
        _ctx.window.draw(_continueButton);
    }
}
