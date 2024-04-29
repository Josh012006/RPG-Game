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


#ifndef RPG_CHARACTER_HPP
#define RPG_CHARACTER_HPP


class Character {
public:
    // Le constructeur
    Character (int type, sf::Texture& texture);

    // Les attributs de l'objet
    int CharacterType;

    // Les méthodes de l'objet
     void run ();
     void walk();
     void setType (int type);
     sf::Sprite getCharacterSprite();
    // Les méthodes de mouvement
     void down ();
     void up ();
     void left ();
     void right ();


private:
    // Les attributs de l'objet
    sf::Sprite CharacterSprite;

    float speed = 0.07f;
    int xperso = 0;
    int yperso = 0;

    int dimension = 48;
    int restriction = 25;
};


#endif //RPG_CHARACTER_HPP
