#pragma once

#include <SFML/Graphics.hpp>


namespace Josh {

    class Window: public sf::RenderWindow {
    public:

        Window() : Window(sf::VideoMode(800, 600), "Default Window") {}
        Window(sf::VideoMode mode, const std::string& title);
        ~Window();

        void StartFade(bool fadeIn, float time, sf::Color color = sf::Color::Black);
        void UpdateFade(float dt);

        void DrawFade();
        bool IsFading() const;




    private:
        sf::RectangleShape _fadeOverlay;
        int _fadeAlpha;
        bool _isFading;
        int _fadeDirection; // -1 pour fadeIn, 1 pour fadeOut
        float _fadeTime;    // Durée totale du fade
        float _fadeSpeed;   // Vitesse en alpha par seconde
    };

}
