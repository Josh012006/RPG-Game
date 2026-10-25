#pragma once

#include <SFML/Graphics.hpp>

// Event helpers. The old InputManager polled sf::Mouse::isButtonPressed inside the event loop:
// a click held for 10 frames fired 10 times. Working from the sf::Event gives exactly one
// activation per press.
namespace Oathbound::input {

    inline bool KeyPressed(const sf::Event& e, sf::Keyboard::Key key) {
        return e.type == sf::Event::KeyPressed && e.key.code == key;
    }

    inline bool LeftClick(const sf::Event& e) {
        return e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left;
    }

    // The window is not resizable and uses the default view, so window pixels == world coordinates.
    inline bool ClickedOn(const sf::Event& e, const sf::Sprite& sprite) {
        return LeftClick(e) &&
               sprite.getGlobalBounds().contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
    }
}
