#pragma once

#include <SFML/Graphics.hpp>

namespace Josh {
    class InputManager {
    public:
        InputManager() {}
        ~InputManager() {}

        // A method to see if a sprite is clicked
        // We use sf::Mouse::Button to see what mouse button was clicked
        // and sf::RenderWindow& to get the window that's concerned by the input event
        bool IsSpriteClicked(sf::Sprite object, sf::Mouse::Button button, sf::RenderWindow& window);

        // A method to get the mouse position on a specific window
        sf::Vector2i GetMousePosition(sf::RenderWindow& window);

        // A method to verify keyboard input: a key being pressed
        bool IsKeyPressed(sf::Keyboard::Key key);

        // A method to verify keyboard input: a key being released
        bool IsKeyReleased(sf::Keyboard::Key key, sf::Event& event);
    };
}
