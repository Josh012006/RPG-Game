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

    // Déclaration des variables pour suivre l'état des touches de déplacement
    bool isMovingUp = false;
    bool isMovingDown = false;
    bool isMovingLeft = false;
    bool isMovingRight = false;

    // Création du personnage
    Character Hero (1, CharacterTexture);



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

                case sf::Event::KeyPressed:
                    switch(event.key.code)
                    {
                        case sf::Keyboard::Up:
                            isMovingUp = true;
                            break;
                        case sf::Keyboard::Down:
                            isMovingDown = true;
                            break;
                        case sf::Keyboard::Left:
                            isMovingLeft = true;
                            break;
                        case sf::Keyboard::Right:
                            isMovingRight = true;
                            break;
                        case sf::Keyboard::D:
                            Hero.run();
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch(event.key.code)
                    {
                        case sf::Keyboard::Up:
                            isMovingUp = false;
                            break;
                        case sf::Keyboard::Down:
                            isMovingDown = false;
                            break;
                        case sf::Keyboard::Left:
                            isMovingLeft = false;
                            break;
                        case sf::Keyboard::Right:
                            isMovingRight = false;
                            break;
                        case sf::Keyboard::D:
                            Hero.walk();
                            break;
                        default:
                            break;
                    }
                    break;

            }
        }


        // Gestion du déplacement vers le bas
        if(isMovingDown && (Hero.getCharacterSprite().getPosition().y < (window.getSize().y - 60))) {
            Hero.down();
        }
        // Gestion du déplacement vers le haut
        if(isMovingUp && (Hero.getCharacterSprite().getPosition().y > 12)) {
            Hero.up();
        }
        // Gestion du déplacement vers la droite
        if(isMovingRight && (Hero.getCharacterSprite().getPosition().x < (window.getSize().x - 60))) {
            Hero.right();
        }
        // Gestion du déplacement vers la gauche
        if(isMovingLeft && (Hero.getCharacterSprite().getPosition().x > 12)) {
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