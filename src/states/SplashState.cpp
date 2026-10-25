#include "states/SplashState.hpp"

#include "engine/Config.hpp"
#include "states/MainMenuState.hpp"

namespace Oathbound {

    void SplashState::OnEnter() {
        _background.setTexture(_ctx.assets.Texture(paths::kSplashBackground));
        const sf::FloatRect b = _background.getGlobalBounds();
        _background.setPosition((config::kScreenWidth - b.width) / 2.0f, (config::kScreenHeight - b.height) / 2.0f);

        _ctx.fade.Start(true, config::kFadeTime);
    }

    void SplashState::HandleEvent(const sf::Event& event) {
        // Any key or click skips the splash.
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
            BeginLeaving();
        }
    }

    void SplashState::BeginLeaving() {
        if (_leaving) return;
        _leaving = true;
        _ctx.fade.Start(false, config::kFadeTime);
    }

    void SplashState::Update(float dt) {
        _elapsed += dt;
        if (_elapsed >= config::kSplashShowTime) BeginLeaving();

        if (_leaving && !_done && !_ctx.fade.IsActive()) {
            _done = true;
            _ctx.states.Replace(std::make_unique<MainMenuState>(_ctx));
        }
    }

    void SplashState::Draw() {
        _ctx.window.draw(_background);
    }
}
