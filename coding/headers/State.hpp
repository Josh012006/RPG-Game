#pragma once

#include <SFML/Graphics.hpp>

namespace Josh {
    class State {
    public:
        // For initializing the state
        virtual void Init() = 0;

        // To handle input from user
        virtual void HandleInput() = 0;
        // Used to manage the update process of the window in that state depending on the input
        virtual void Update(float dt) = 0;
        // Drawing the changes : dt is for managing the speed of the drawing
        virtual void Draw(float dt) = 0;


        virtual void Pause() {}
        virtual void Resume() {}

        virtual ~State() {}

    protected:
        // Used to keep track of how long the state has been displayed
        sf::Clock _clock;

        // A boolean to know if the fadeOut has started
        bool _fadeOut = false;

        // The fadeTime
        float _fadeTime = 1.5f;

    };
}
