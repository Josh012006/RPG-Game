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

        // We verify if the defined duration of this has already been taken
        if(this->_clock.getElapsedTime().asSeconds() > SPLASH_STATE_SHOW_TIME) {

            // Transition
            if (!this->_data->window.IsFading()) {
                float fadeTime = 1.5f;
                this->_data->window.StartFade(false, fadeTime); // Fade out

                if(this->_clock.getElapsedTime().asSeconds() > SPLASH_STATE_SHOW_TIME + fadeTime) {
                    // Switch to the main Menu
                    this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
                }
            }

        }

        // Updating the fade
        std::cout << "took here" << std::endl;
        this->_data->window.UpdateFade(dt);

    }

    void SplashState::Draw(float dt) {
        this->_data->window.clear(sf::Color::Black);

        this->_data->window.draw(this->_background);
        this->_data->window.DrawFade();

        this->_data->window.display();
    }
}
