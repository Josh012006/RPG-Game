#include <sstream>
#include "../headers/SplashState.hpp"
#include "../headers/MainMenuState.hpp"
#include "../headers/DEFINITIONS.hpp"

#include <iostream>

namespace Josh {

    // Assigning the given data to the data attribute of the class
    SplashState::SplashState(GameDataRef data) : _data(data) {
        // Add any other necessary implementation here
        std::cout << "SplashState loaded successfully" << std::endl;
    }

    void SplashState::Init() {
        // Loading the background image
        this->_data->assets.LoadTexture("Splash State Background", SPLASH_STATE_BACKGROUND_FILEPATH);

        // Now setting the background's sprite
        this->_background.setTexture(this->_data->assets.GetTexture("Splash State Background"));

        // Position the background
        this->_background.setPosition((SCREEN_WIDTH / 2) - (this->_background.getGlobalBounds().width / 2),
                                 (SCREEN_HEIGHT / 2) - (this->_background.getGlobalBounds().height / 2));

        // Start the fade in effect
        this->_data->window.StartFade(true, this->_fadeTime);
    }

    void SplashState::HandleInput() {
        sf::Event event;

        // The sole event important with the Splash State is the closing event
        while(this->_data->window.pollEvent(event)) {
            if(sf::Event::Closed == event.type) {
                this->_data->window.close();
            }
        }
    }

    void SplashState::Update(float dt) {

        float elapsedTime = this->_clock.getElapsedTime().asSeconds();

        // We verify if the defined duration of this has already been taken
        if(elapsedTime > SPLASH_STATE_SHOW_TIME) {

            bool fadeOutEnd = elapsedTime > (SPLASH_STATE_SHOW_TIME + this->_fadeTime);

            if(fadeOutEnd) {
                // Update the fadeOut variable to prevent the background from being drawn
                this->_fadeOut = true;
            }

            // Transition
            if (!this->_data->window.IsFading()) {

                this->_data->window.StartFade(false, this->_fadeTime); // Fade out

                if(fadeOutEnd) {
                    // Switch to the main Menu
                    this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
                }
            }

        }

        // Updating the fade
        this->_data->window.UpdateFade(dt);

    }

    void SplashState::Draw(float dt) {
        this->_data->window.clear(sf::Color::Black);

        if (!this->_fadeOut) {
            this->_data->window.draw(this->_background);
        }
        this->_data->window.DrawFade();

        this->_data->window.display();
    }
}
