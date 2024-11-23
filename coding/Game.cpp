#include "headers/Game.hpp"
#include "headers/SplashState.hpp"

namespace Josh {
    Game::Game(int width, int height, std::string title) {
        // Creating the window
        _data->window.create(sf::VideoMode(width, height), title);

        // Implementing our first state
        _data->machine.AddState(StateRef (new SplashState(this->_data)));

        // Running the game
        this->Run();
    }

    void Game::Run() {
        float newTime, frameTime, interpolation;

        // Elapsed time since the start of the game
        float currentTime = this->_clock.getElapsedTime().asSeconds();

        float accumulator = 0.0f;

        // Managing the game loop
        while(this->_data->window.isOpen()) {

            // We first process any change in the states
            this->_data->machine.ProcessStateChanges();

            // We keep track of the time
            newTime = this->_clock.getElapsedTime().asSeconds();

            // Keep track of the frame time to handle any difference in the frame time
            frameTime = newTime - currentTime;
            // Keeping a reasonable frame time
            if(frameTime > 0.25f) {
                frameTime = 0.25f;
            }

            // Updating the currentTime and the accumulator
            currentTime = newTime;
            accumulator += frameTime;

            // Verifying the frame rate
            while(accumulator >= dt) {
                // The top state handles the input from the user
                this->_data->machine.GetActiveState()->HandleInput();

                // Updating the window
                this->_data->machine.GetActiveState()->Update(dt);

                accumulator -= dt;
            }

            interpolation = accumulator / dt;
            this->_data->machine.GetActiveState()->Draw(interpolation);
        }
    }
}
