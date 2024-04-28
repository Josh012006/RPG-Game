#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "headers/character.hpp"


int main()
{

    //Enabling antialiases
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    //Creating our game's window
    sf::RenderWindow window(sf::VideoMode(1200, 700), "RPG Game");

    // Creating the character textures
    sf::Texture CharacterTexture;
    CharacterTexture.loadFromFile("media/hero.png");



    while(window.isOpen())
    {
        //Checking for events
        sf::Event event;

        while(window.pollEvent(event))
        {
            switch (event.type)
            {
                case (sf::Event::Closed):
                    window.close();
                    break;
            }
        }

        Character Hero (1, CharacterTexture);

        // Gestion du déplacement vers le bas
        while((sf::Keyboard::isKeyPressed((sf::Keyboard::Down))) && (Hero.getCharacterSprite().getPosition().y < (window.getSize().y - 60))) {
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::B))) {
                Hero.run();
            }
            Hero.down();
        }
        // Gestion du déplacement vers le haut
        if((sf::Keyboard::isKeyPressed((sf::Keyboard::Up))) && (Hero.getCharacterSprite().getPosition().y > 12)) {
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::B))) {
                Hero.run();
            }
            Hero.up();
        }
        // Gestion du déplacement vers la droite
        if((sf::Keyboard::isKeyPressed((sf::Keyboard::Right))) && (Hero.getCharacterSprite().getPosition().x < (window.getSize().x - 60))) {
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::B))) {
                Hero.run();
            }
            Hero.right();
        }
        // Gestion du déplacement vers la gauche
        if((sf::Keyboard::isKeyPressed((sf::Keyboard::Left))) && (Hero.getCharacterSprite().getPosition().x > 12)) {
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::B))) {
                Hero.run();
            }
            Hero.left();
        }

        //Clearing the screen
        window.clear();

        //Draw the hero
        window.draw(Hero.getCharacterSprite());

        //Displaying the frames
        window.display();
    }

    //Program's end
    return 0;
}