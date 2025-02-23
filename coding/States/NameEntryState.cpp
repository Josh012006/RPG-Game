#include <sstream>

#include "../headers/NameEntryState.hpp"
#include "../headers/DEFINITIONS.hpp"

#include <iostream>



namespace Josh {

    // Assigning the given data to the data attribute of the class
    NameEntryState::NameEntryState(GameDataRef data) : _data(data) {
        // Add any other necessary implementation here
        std::cout << "NameEntryState loaded successfully" << std::endl;
    }

    void NameEntryState::Init() {

        // Load and set textures

        this->_data->assets.LoadTexture("NameEntry", "media/StateResources/NameEntryState/background.png");
        this->_data->assets.LoadTexture("BackgroundLayout", BACKGROUND_LAYOUT);
        this->_data->assets.LoadTexture("StartGameButton", "media/StateResources/NameEntryState/startGameButton.png");

        //this->_background.setTexture(this->_data->assets.GetTexture("NameEntry"));
        //this->_backgroundLayout.setTexture(this->_data->assets.GetTexture("BackgroundLayout"));
        //this->_startGameButton.setTexture(this->_data->assets.GetTexture("StartGameButton"));

        // Setting sprites position

        //this->_startGameButton.setPosition((SCREEN_WIDTH / 2) - (this->_continueGameButton.getGlobalBounds().width / 2),
                                              //(SCREEN_HEIGHT / 2) - (this->_continueGameButton.getGlobalBounds().height / 2) + 150);

        // Load the font

        this->_data->assets.LoadFont("MainFont", GAME_FONT);
        this->_mainFont = this->_data->assets.GetFont("MainFont");


        // Start the fade in effect
        //this->_data->window.StartFade(true, this->_fadeTime);
    }


    void NameEntryState::HandleInput() {
        sf::Event event;

        // The sole event important with the Splash State is the closing event
        while(this->_data->window.pollEvent(event)) {
            if(sf::Event::Closed == event.type) {
                this->_data->window.close();
            }

            if(this->_data->input.IsSpriteClicked(this->_startGameButton, sf::Mouse::Left, this->_data->window)) {
                // Make an initial save

                // Start the introductory story state
                //this->_data->machine.AddState(StateRef(new NameEntryState(_data)), true);
            }
        }
    }

    void NameEntryState::Update(float dt) {

        // Updating the fade
        // this->_data->window.UpdateFade(dt);
    }


    void NameEntryState::Draw(float dt) {
        this->_data->window.clear();

        this->_data->window.draw(this->_background);
        this->_data->window.draw(this->_backgroundLayout);
        this->_data->window.draw(this->_startGameButton);

        // this->_data->window.DrawFade();


        this->_data->window.display();
    }


}