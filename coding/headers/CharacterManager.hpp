#pragma once

#include "AssetManager.hpp"

#include <SFML/Graphics.hpp>

namespace Josh{
    class CharacterManager{
    public:
        CharacterManager() {}
        ~CharacterManager() {}

        // Method to generate the Sprite of the Character
        void GenerateCharacter(int type, std::string characterName, std::string fileName, Josh::AssetManager& assetManager, int dimensionX, int dimensionY);

        // Method to manage the xPos and yPos variables
        void ManageSpriteArea();

        // Method to change the character type
        void SetCharacterType(int type);

        // Method to manage the character speed
        void IsRunning(bool condition);

        // Method that returns the character sprite for it to be drawn
        sf::Sprite& GetCharacterSprite();

        // Managing character's movements
        void MoveUp();
        void MoveDown();
        void MoveLeft();
        void MoveRight();

    private:
        sf::Sprite CharacterSprite;

        Josh::AssetManager AssetManager;

        std::string CharacterName;
        std::string FileName;
        int CharacterType = 1;

        float CharacterSpeed = 0.03f;

        int xPos = 0;
        int yPos = 0;

        int DimensionX = 48;
        int DimensionY = 48;
        int Restriction = 25;
    };
}
