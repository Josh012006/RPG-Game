#include "../headers/Window.hpp"
#include "../headers/DEFINITIONS.hpp"
#include <iostream>

namespace Josh {

    Window::Window(sf::VideoMode mode, const std::string& title)
        : sf::RenderWindow(mode, title), _fadeAlpha(0), _isFading(false), _fadeDirection(0), _fadeTime(1.0f){
        this->_fadeOverlay.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
        this->_fadeOverlay.setFillColor(sf::Color(0, 0, 0, this->_fadeAlpha));
    }

    Window::~Window() {}

    void Window::StartFade(bool fadeIn, float time, sf::Color color) {
        this->_isFading = true;
        this->_fadeDirection = fadeIn ? -1 : 1; // -1 for fadeIn, 1 for fadeOut
        this->_fadeTime = time;
        this->_fadeSpeed = 255.0f / (this->_fadeTime * 60.0f); // Supposing 60 FPS
        this->_fadeAlpha = fadeIn ? 255 : 0;
        color.a = this->_fadeAlpha;
        this->_fadeOverlay.setFillColor(color);
    }

    void Window::UpdateFade(float dt) {
        if (_isFading) {
            // Updating the fade progression depending on the time
            _fadeAlpha += _fadeDirection * _fadeSpeed;

            // The fadeAlpha values must be limited between 0 and 255
            if (_fadeAlpha < 0) {
                _fadeAlpha = 0;
            } else if (_fadeAlpha > 255) {
                _fadeAlpha = 255;
            }

            // Apply the fadeOverLay's color to fade
            sf::Color currentColor = this->_fadeOverlay.getFillColor();
            currentColor.a = static_cast<sf::Uint8>(this->_fadeAlpha);
            this->_fadeOverlay.setFillColor(currentColor);

            // If the fade has ended, stop the effect
            if (_fadeAlpha <= 0 || _fadeAlpha >= 255) {
                _isFading = false;
            }
        }
    }

    void Window::DrawFade() {
        if (this->_isFading) {
            this->draw(this->_fadeOverlay);
        }
    }

    bool Window::IsFading() const {
        return this->_isFading;
    }

}
