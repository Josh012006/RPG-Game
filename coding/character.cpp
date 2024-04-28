#include "headers/character.hpp"


Character::Character(int type, sf::Texture& texture) {
    // Setting the sprite's texture
    CharacterSprite.setTexture(texture);
    CharacterSprite.setPosition(400, 300);

    // Assigning the character type
    this -> setType(type);
}

sf::Sprite Character::getCharacterSprite () {
    return CharacterSprite;
}

void Character:: run () {
    speed = 0.1f;
}

void Character:: setType (int type) {
    // Assignation du type
    CharacterType = type;

    // Change xperso et yperso pour changer l'image
    switch (type) {
        case 1:
            xperso = 0;
            yperso = 0;
            break;

        case 2:
            xperso = 144;
            yperso = 0;
            break;

        case 3:
            xperso = 288;
            yperso = 0;
            break;

        case 4:
            xperso = 432;
            yperso = 0;
            break;

        case 5:
            xperso = 0;
            yperso = 192;
            break;

        case 6:
            xperso = 144;
            yperso = 192;
            break;

        case 7:
            xperso = 288;
            yperso = 192;
            break;

        case 8:
            xperso = 432;
            yperso = 192;
            break;

        default:
            xperso = 0;
            yperso = 0;
            break;
    }


    // Assigner les coordonnées de la texture
    CharacterSprite.setTextureRect(sf::IntRect(xperso, yperso, dimension, dimension));
}


// Les méthodes pour les mouvements
void Character:: down () {
    xperso = ((int)(CharacterSprite.getPosition().y / restriction)) % 3;
    xperso = xperso * dimension;

    CharacterSprite.setTextureRect(sf::IntRect(xperso, yperso, dimension, dimension));
    CharacterSprite.move(0, speed);
}

void Character:: up () {
    xperso = ((int)(CharacterSprite.getPosition().y / restriction)) % 3;
    xperso = xperso * dimension;

    CharacterSprite.setTextureRect(sf::IntRect(xperso, yperso + (dimension * 3), dimension, dimension));
    CharacterSprite.move(0, -speed);
}

void Character:: left () {
    xperso = ((int)(CharacterSprite.getPosition().x / restriction)) % 3;
    xperso = xperso * dimension;

    CharacterSprite.setTextureRect(sf::IntRect(xperso, yperso + dimension, dimension, dimension));
    CharacterSprite.move(-speed, 0);
}

void Character:: right () {
    xperso = ((int)(CharacterSprite.getPosition().x / restriction)) % 3;
    xperso = xperso * dimension;

    CharacterSprite.setTextureRect(sf::IntRect(xperso, yperso + (dimension * 2), dimension, dimension));
    CharacterSprite.move(speed, 0);
}