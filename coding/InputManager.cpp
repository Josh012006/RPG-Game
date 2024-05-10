#include "headers/InputManager.hpp"

namespace Josh {
    bool InputManager::IsSpriteClicked(sf::Sprite object, sf::Mouse::Button button, sf::RenderWindow& window) {
        if(sf::Mouse::isButtonPressed(button)) {
            // We create a temporary rectangle for the sprite
            sf::IntRect tempRect(object.getPosition().x, object.getPosition().y, object.getGlobalBounds().width, object.getGlobalBounds().height);

            // We check if this temporary rectangle contains the Mouse
            if(tempRect.contains(sf::Mouse::getPosition(window))) {
                return true;
            }
        }

        return false;
    }

    sf::Vector2i InputManager::GetMousePosition(sf::RenderWindow& window) {
        return sf::Mouse::getPosition(window);
    }

    bool InputManager::IsKeyPressed(sf::Keyboard::Key key) {
        if(sf::Keyboard::isKeyPressed(key)) {
            return true;
        }
        return false;
    }

    bool InputManager::IsKeyReleased(sf::Keyboard::Key key, sf::Event& event) {
        // Given that sf::KeyBoard doesn't have any function isKeyReleased, I use sf::Event with it to resolve
        // my problem
        if(event.type == sf::Event::KeyReleased) {
            if(key == event.key.code) {
                return true;
            }
        }
        return false;
    }
}
