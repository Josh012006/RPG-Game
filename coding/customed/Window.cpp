#include "headers/Window.hpp"


namespace Josh {

    Window::Window(sf::VideoMode mode, const std::string& title)
        : sf::RenderWindow(mode, title), _fadeAlpha(0), _isFading(false), _fadeDirection(0), _fadeTime(1.0f){
        this->_fadeOverlay.setSize(sf::Vector2f(mode.width, mode.height));
        this->_fadeOverlay.setFillColor(sf::Color(0, 0, 0, this->_fadeAlpha));
    }

    void Window::StartFade(bool fadeIn, float time, sf::Color color) {
        this->_isFading = true;
        this->_fadeDirection = fadeIn ? -1 : 1; // -1 pour fadeIn, 1 pour fadeOut
        this->_fadeTime = time;
        this->_fadeSpeed = 255.0f / (this->_fadeTime * 60.0f); // Supposant 60 FPS
        this->_fadeOverlay.setFillColor(color);
        this->_fadeAlpha = fadeIn ? 255 : 0;
    }

    void Window::UpdateFade(float dt) {
        if (this->_isFading) {
            this->_fadeAlpha += static_cast<int>(this->_fadeDirection * this->_fadeSpeed * dt * 60.0f);
            if (this->_fadeAlpha <= 0 || this->_fadeAlpha >= 255) {
                this->_fadeAlpha = std::clamp(this->_fadeAlpha, 0, 255);
                this->_isFading = false;
            }
            this->_fadeOverlay.setFillColor(sf::Color(
                this->_fadeOverlay.getFillColor().r,
                this->_fadeOverlay.getFillColor().g,
                this->_fadeOverlay.getFillColor().b,
                this->_fadeAlpha));
        }
    }

    void Window::DrawFade() {
        if (this->_fadeAlpha > 0) {
            this->draw(this->_fadeOverlay);
        }
    }

    bool Window::IsFading() const {
        return this->_isFading;
    }

}