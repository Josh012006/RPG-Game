#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "engine/Config.hpp"

namespace Oathbound {

    // Where to find the hero in the sprite sheet. media/hero.png is 576x384: 8 characters laid out
    // 4 across x 2 down, each one a block of 3 frames x 4 directions of 48x48 pixels.
    struct AvatarSpec {
        std::string texturePath = paths::kHeroSheet;
        int frameWidth = 48;
        int frameHeight = 48;
        int character = 1;  // 1..8, same numbering as the old CharacterManager "type"
    };

    // The drawable, animated hero. It replaces CharacterManager, with three fixes:
    //  - it no longer owns a copy of the AssetManager (the sprite used to point into that copy),
    //  - movement is expressed in pixels per second and scaled by dt (the old code moved a fixed
    //    0.03 px per call, so the speed depended on how often the loop ran),
    //  - animation is driven by elapsed time instead of being derived from the position.
    class PlayerAvatar {
    public:
        // `texture` must outlive the avatar (it is owned by the AssetManager).
        void Setup(const sf::Texture& texture, const AvatarSpec& spec);

        // direction: raw input vector (components in -1..1). It is normalised internally so that
        // moving diagonally is not faster.
        void Update(float dt, sf::Vector2f direction, bool running, const sf::FloatRect& bounds);

        void SetPosition(sf::Vector2f position) { _sprite.setPosition(position); }
        sf::Vector2f Position() const { return _sprite.getPosition(); }
        void Draw(sf::RenderTarget& target) const { target.draw(_sprite); }

    private:
        enum class Facing { Down = 0, Left = 1, Right = 2, Up = 3 };  // row order in the sheet

        void ApplyFrame(int column);

        sf::Sprite _sprite;
        AvatarSpec _spec;
        sf::Vector2i _origin{0, 0};  // top-left of this character's block in the sheet
        Facing _facing = Facing::Down;
        float _animTime = 0.0f;

        static constexpr float kWalkSpeed = 140.0f;   // px / s
        static constexpr float kRunSpeed = 260.0f;
        static constexpr float kFramesPerSecond = 8.0f;
    };
}
