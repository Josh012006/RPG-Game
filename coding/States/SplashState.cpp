#include <sstream>
#include "../headers/SplashState.hpp"
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
        _background.setTexture(this->_data->assets.GetTexture("Splash State Background"));
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
            // Switch to the main Menu
            std::cout << "SplashState ended. Go to Main Menu." << std::endl;
        }
    }

    void SplashState::Draw(float dt) {
        this->_data->window.clear(sf::Color::Cyan);
        this->_data->window.draw(this->_background);
        this->_data->window.display();
    }
}
