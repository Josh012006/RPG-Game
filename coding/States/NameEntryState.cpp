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

        this->_data->assets.LoadTexture("PaperTexture", "media/paper_texture.png");
        this->_data->assets.LoadTexture("BackgroundLayout", BACKGROUND_LAYOUT);
        this->_data->assets.LoadTexture("StartGameButton", BUTTON_LAYOUT);
        this->_data->assets.LoadTexture("NameEntry", "media/StateResources/NameEntryState/writingSpace.png");

        this->_background.setTexture(this->_data->assets.GetTexture("PaperTexture"));
        this->_backgroundLayout.setTexture(this->_data->assets.GetTexture("BackgroundLayout"));
        this->_startGameButton.setTexture(this->_data->assets.GetTexture("StartGameButton"));
        this->_nameEntrySpace.setTexture(this->_data->assets.GetTexture("NameEntry"));

        // Setting sprites position

        this->_startGameButton.setPosition((SCREEN_WIDTH / 2) - (this->_startGameButton.getGlobalBounds().width / 2) + 450,
                                              (SCREEN_HEIGHT / 2) - (this->_startGameButton.getGlobalBounds().height / 2) + 350);

        this->_nameEntrySpace.setPosition((SCREEN_WIDTH / 2) - (this->_nameEntrySpace.getGlobalBounds().width / 2), 300);

        // Load the font

        this->_data->assets.LoadFont("MainFont", GAME_FONT);
        this->_mainFont = this->_data->assets.GetFont("MainFont");

        // Writing the text for the button

        this->_startGameText.setFont(this->_mainFont);
        this->_startGameText.setCharacterSize(25);
        this->_startGameText.setString("Start game");
        this->_startGameText.setPosition((SCREEN_WIDTH / 2) - (this->_startGameButton.getGlobalBounds().width / 2) + 520,
                           (SCREEN_HEIGHT / 2) - (this->_startGameButton.getGlobalBounds().height / 2) + 365);


        // Writing the text for the instructions

        this->_instructions.setFont(this->_mainFont);
        this->_instructions.setCharacterSize(30);
        const sf::Color mainColor = sf::Color(58, 18, 16);
        this->_instructions.setFillColor(mainColor);
        this->_instructions.setString("Please enter your character's name. Use your keyboard.");
        this->_instructions.setPosition((SCREEN_WIDTH / 2) - 370, 170);


        // Configuring the text for the character's name

        this->_characterName.setFont(this->_mainFont);
        this->_characterName.setCharacterSize(25);
        this->_characterName.setString("");
        this->_characterName.setPosition((SCREEN_WIDTH / 2) - (this->_nameEntrySpace.getGlobalBounds().width / 2) + 85, 327);


        // Start the fade in effect
        //this->_data->window.StartFade(true, this->_fadeTime);
    }


    void NameEntryState::HandleInput() {
        sf::Event event;

        // Managing the input for the name entry
        while(this->_data->window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) {
                this->_data->window.close();
            }

            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Backspace) {
                    if(this->_characterName.getString().getSize() > 0) {
                        std::string modifiedName = this->_characterName.getString().toAnsiString();
                        modifiedName = modifiedName.substr(0, modifiedName.size() - 1);
                        this->_characterName.setString(modifiedName);
                    }
                }

            }
            else if(event.type == sf::Event::TextEntered) {

                if (event.text.unicode == 8)  // 8 = Backspace en ASCII
                    return;  // Ignore Backspace dans TextEntered

                std::cout << "Took here" << std::endl;

                if(event.text.unicode < 128) {
                    char c = static_cast<char>(event.text.unicode);

                    if(this->_characterName.getString().getSize() < 20) {
                        this->_characterName.setString(this->_characterName.getString() + sf::String(c));
                    }
                }
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

        this->_data->window.draw(this->_startGameText);
        this->_data->window.draw(this->_instructions);

        this->_data->window.draw(this->_nameEntrySpace);
        this->_data->window.draw(this->_characterName);

        // this->_data->window.DrawFade();


        this->_data->window.display();
    }


}