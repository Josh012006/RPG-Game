#include "headers/CharacterManager.hpp"

namespace Josh {
    void CharacterManager::GenerateCharacter(int type, std::string characterName, std::string fileName, Josh::AssetManager& assetManager, int dimensionX, int dimensionY) {
        // We set the dimensions
        this->DimensionX = dimensionX;
        this->DimensionY = dimensionY;

        // We set the FileName
        this->FileName = fileName;

        // We set the assetManager
        this->AssetManager = assetManager;

        // We set the CharacterType and its name
        this->CharacterType = type;
        this->CharacterName = characterName;

        // We first create the texture with the AssetManager methods
        this->AssetManager.LoadTexture(this->CharacterName, this->FileName);
        // Then we call the ManageSpriteArea method
        this->ManageSpriteArea();

        // Then we get it and we set it as the Sprite's texture
        this->CharacterSprite.setTexture(this->AssetManager.GetTexture((this->CharacterName)));
        this->CharacterSprite.setTextureRect(sf::IntRect (this->xPos, this->yPos, this->DimensionX, this->DimensionY));
    }

    void CharacterManager::ManageSpriteArea() {
        switch (this->CharacterType) {
            case 1:
                this->xPos = 0;
                this->yPos = 0;
                break;

            case 2:
                this->xPos = (this->DimensionX) * 3;
                this->yPos = 0;
                break;

            case 3:
                this->xPos = (this->DimensionX) * 6;
                this->yPos = 0;
                break;

            case 4:
                this->xPos = (this->DimensionX) * 9;
                this->yPos = 0;
                break;

            case 5:
                this->xPos = 0;
                this->yPos = (this->DimensionY) * 4;
                break;

            case 6:
                this->xPos = (this->DimensionX) * 3;
                this->yPos = (this->DimensionY) * 4;
                break;

            case 7:
                this->xPos = (this->DimensionX) * 6;
                this->yPos = (this->DimensionY) * 4;
                break;

            case 8:
                this->xPos = (this->DimensionX) * 9;
                this->yPos = (this->DimensionY) * 4;
                break;

            default:
                this->xPos = 0;
                this->yPos = 0;
                break;
        }
    }

    void CharacterManager::SetCharacterType(int type) {
        // We modify the type
        this->CharacterType = type;

        // We then change the texture in the AssetManager
        this->GenerateCharacter(this->CharacterType, this->CharacterName, this->FileName, this->AssetManager, this->DimensionX, this->DimensionY);
    }

    void CharacterManager::IsRunning(bool condition) {
        if(condition) {
            this->CharacterSpeed = 0.06f;
        }
        else {
            this->CharacterSpeed = 0.03f;
        }
    }

    sf::Sprite& CharacterManager::GetCharacterSprite() {
        return this->CharacterSprite;
    }


    // Movements methods
    void CharacterManager::MoveUp() {
        // Generating a random number that's supposed to help manage the animation
        int considered = ((int)((this->CharacterSprite).getPosition().y / this->Restriction)) % 3;
        considered = (this->xPos) + (considered * (this->DimensionX));

        // Changing the texture part to show for this step of the animation
        (this->CharacterSprite).setTextureRect(sf::IntRect(considered, ((this->yPos) + ((this->DimensionY) * 3)), this->DimensionX, this->DimensionY));

        // Make the sprite move up
        (this->CharacterSprite).move(0, -(this->CharacterSpeed));
    }
    void CharacterManager::MoveDown() {
        // Generating a random number that's supposed to help manage the animation
        int considered = ((int)((this->CharacterSprite).getPosition().y / this->Restriction)) % 3;
        considered = (this->xPos) + (considered * (this->DimensionX));

        // Changing the texture part to show for this step of the animation
        (this->CharacterSprite).setTextureRect(sf::IntRect(considered, this->yPos, this->DimensionX, this->DimensionY));

        // Make the sprite move down
        (this->CharacterSprite).move(0, this->CharacterSpeed);
    }
    void CharacterManager::MoveLeft() {
        // Generating a random number that's supposed to help manage the animation
        int considered = ((int)((this->CharacterSprite).getPosition().x / this->Restriction)) % 3;
        considered = (this->xPos) + (considered * (this->DimensionX));

        // Changing the texture part to show for this step of the animation
        (this->CharacterSprite).setTextureRect(sf::IntRect(considered, ((this->yPos) + (this->DimensionY)), this->DimensionX, this->DimensionY));

        // Make the sprite move left
        (this->CharacterSprite).move(-(this->CharacterSpeed), 0);
    }
    void CharacterManager::MoveRight() {
        // Generating a random number that's supposed to help manage the animation
        int considered = ((int)((this->CharacterSprite).getPosition().x / this->Restriction)) % 3;
        considered = (this->xPos) + (considered * (this->DimensionX));

        // Changing the texture part to show for this step of the animation
        (this->CharacterSprite).setTextureRect(sf::IntRect(considered, ((this->yPos) + ((this->DimensionY) * 2)), this->DimensionX, this->DimensionY));

        // Make the sprite move right
        (this->CharacterSprite).move((this->CharacterSpeed), 0);
    }
}
