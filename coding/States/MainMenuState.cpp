#include <sstream>

#include "../headers/MainMenuState.hpp"
#include "../headers/DEFINITIONS.hpp"

#include <iostream>



namespace Josh {

    // Assigning the given data to the data attribute of the class
    MainMenuState::MainMenuState(GameDataRef data) : _data(data) {
        // Add any other necessary implementation here
        std::cout << "MainMenuState loaded successfully" << std::endl;
    }

    void MainMenuState::Init() {
        this->_data->assets.LoadTexture("MainMenu", MAIN_MENU_BACKGROUND_FILEPATH);
        this->_data->assets.LoadTexture("MainMenuTitle", MAIN_MENU_TITLE_PATH);
        this->_data->assets.LoadTexture("NewGameButton", MAIN_MENU_NEW_GAME_BUTTON);
        this->_data->assets.LoadTexture("ContinueGameButton", MAIN_MENU_CONTINUE_BUTTON);

        this->_background.setTexture(this->_data->assets.GetTexture("MainMenu"));
        this->_title.setTexture(this->_data->assets.GetTexture("MainMenuTitle"));
        this->_newGameButton.setTexture(this->_data->assets.GetTexture("NewGameButton"));
        this->_continueGameButton.setTexture(this->_data->assets.GetTexture("ContinueGameButton"));

        this->_title.setPosition((SCREEN_WIDTH / 2) - (this->_title.getGlobalBounds().width / 2),
                                 (SCREEN_HEIGHT / 2) - (this->_title.getGlobalBounds().height / 2) - 250);
        this->_newGameButton.setPosition((SCREEN_WIDTH / 2) - (this->_newGameButton.getGlobalBounds().width / 2),
                                 (SCREEN_HEIGHT / 2) - (this->_newGameButton.getGlobalBounds().height / 2));
        this->_continueGameButton.setPosition((SCREEN_WIDTH / 2) - (this->_continueGameButton.getGlobalBounds().width / 2),
                                 (SCREEN_HEIGHT / 2) - (this->_continueGameButton.getGlobalBounds().height / 2) + 300);
    }


    void MainMenuState::HandleInput() {
        sf::Event event;

        // The sole event important with the Splash State is the closing event
        while(this->_data->window.pollEvent(event)) {
            if(sf::Event::Closed == event.type) {
                this->_data->window.close();
            }

            if(this->_data->input.IsSpriteClicked(this->_newGameButton, sf::Mouse::Left, this->_data->window)) {
                std::cout << "Start a new Game" << std::endl;
            }

            if(this->_data->input.IsSpriteClicked(this->_continueGameButton, sf::Mouse::Left, this->_data->window)) {
                std::cout << "Load the saves." << std::endl;
            }
        }
    }

    void MainMenuState::Update(float dt) {

    }


    void MainMenuState::Draw(float dt) {
        this->_data->window.clear();

        this->_data->window.draw(this->_background);
        this->_data->window.draw(this->_title);
        this->_data->window.draw(this->_newGameButton);
        this->_data->window.draw(this->_continueGameButton);

        this->_data->window.display();
    }


}