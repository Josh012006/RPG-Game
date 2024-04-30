#include "headers/character.hpp"


// Le constructeur de la fonction. Il a deux rôles:
// Construire le personnage et lui assigner son type
Character::Character(int type, sf::Texture& texture) {
    // Setting the sprite's texture
    CharacterSprite.setTexture(texture);
    CharacterSprite.setPosition(400, 300);

    // Assigning the character type
    this -> setType(type);
}

// Retourne le sprite du caractère pour qu'il puisse être utilisé dans la fonction main
// pour être dessiner sur la fenêtre
sf::Sprite Character::getCharacterSprite () {
    return CharacterSprite;
}


// Réguler la vitesse
void Character:: run () {
    speed = 0.06f;
}

void Character:: walk () {
    speed = 0.03f;
}


// Pour définir le type de personnage à utiliser
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
    // Génération d'un nombre qui permet de faire défiler les images pour pouvoir faire une animation
    int considered = ((int)(CharacterSprite.getPosition().y / restriction)) % 3;
    considered = xperso + considered * dimension;

    // Réinitialisation donc de la partie de l'image à montrer pour l'animation
    CharacterSprite.setTextureRect(sf::IntRect(considered, yperso, dimension, dimension));
    CharacterSprite.move(0, speed);
}

void Character:: up () {
    // Génération d'un nombre qui permet de faire défiler les images pour pouvoir faire une animation
    int considered = ((int)(CharacterSprite.getPosition().y / restriction)) % 3;
    considered = xperso + considered * dimension;

    // Réinitialisation donc de la partie de l'image à montrer pour l'animation
    CharacterSprite.setTextureRect(sf::IntRect(considered, yperso + (dimension * 3), dimension, dimension));
    CharacterSprite.move(0, -speed);
}

void Character:: left () {
    // Génération d'un nombre qui permet de faire défiler les images pour pouvoir faire une animation
    int considered = ((int)(CharacterSprite.getPosition().x / restriction)) % 3;
    considered = xperso + considered * dimension;

    // Réinitialisation donc de la partie de l'image à montrer pour l'animation
    CharacterSprite.setTextureRect(sf::IntRect(considered, yperso + dimension, dimension, dimension));
    CharacterSprite.move(-speed, 0);
}

void Character:: right () {
    // Génération d'un nombre qui permet de faire défiler les images pour pouvoir faire une animation
    int considered = ((int)(CharacterSprite.getPosition().x / restriction)) % 3;
    considered = xperso + considered * dimension;

    // Réinitialisation donc de la partie de l'image à montrer pour l'animation
    CharacterSprite.setTextureRect(sf::IntRect(considered, yperso + (dimension * 2), dimension, dimension));
    CharacterSprite.move(speed, 0);
}