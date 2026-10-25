#pragma once

#include <SFML/Graphics.hpp>

namespace Oathbound {

    // Full-screen fade to/from a colour. Replaces the fade code that used to live inside a
    // class deriving from sf::RenderWindow. It is time based (the old version assumed 60 FPS
    // and truncated its speed to an int, so fades lasted longer than requested).
    //
    // After a fade-OUT the screen stays covered until the next Start(): the following state is
    // expected to fade back in.
    class ScreenFade {
    public:
        // fadeIn=true : black -> scene.   fadeIn=false : scene -> black.
        void Start(bool fadeIn, float durationSeconds, sf::Color color = sf::Color::Black);
        void Update(float dt);
        void Draw(sf::RenderTarget& target) const;

        bool IsActive() const { return _active; }

    private:
        float _alpha = 0.0f;  // 0..255
        float _speed = 0.0f;  // alpha per second
        bool _fadingIn = true;
        bool _active = false;
        sf::Color _color = sf::Color::Black;
    };
}
