#include "engine/ScreenFade.hpp"

#include <algorithm>

namespace Oathbound {

    void ScreenFade::Start(bool fadeIn, float durationSeconds, sf::Color color) {
        // Interrupting a running fade continues from the current opacity (no visual jump).
        if (!_active) _alpha = fadeIn ? 255.0f : 0.0f;
        _fadingIn = fadeIn;
        _speed = 255.0f / std::max(durationSeconds, 0.001f);
        _color = color;
        _active = true;
    }

    void ScreenFade::Update(float dt) {
        if (!_active) return;

        _alpha += (_fadingIn ? -_speed : _speed) * dt;
        if (_fadingIn && _alpha <= 0.0f) { _alpha = 0.0f; _active = false; }
        if (!_fadingIn && _alpha >= 255.0f) { _alpha = 255.0f; _active = false; }
    }

    void ScreenFade::Draw(sf::RenderTarget& target) const {
        if (_alpha <= 0.0f) return;

        sf::RectangleShape overlay(sf::Vector2f(target.getSize()));
        overlay.setFillColor(sf::Color(_color.r, _color.g, _color.b, static_cast<sf::Uint8>(_alpha)));
        target.draw(overlay);
    }
}
