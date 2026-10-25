#include "engine/PlayerAvatar.hpp"

#include <algorithm>
#include <cmath>

namespace Oathbound {

    void PlayerAvatar::Setup(const sf::Texture& texture, const AvatarSpec& spec) {
        _spec = spec;
        const int index = (spec.character >= 1 && spec.character <= 8) ? spec.character - 1 : 0;
        _origin.x = (index % 4) * 3 * spec.frameWidth;
        _origin.y = (index / 4) * 4 * spec.frameHeight;

        _sprite.setTexture(texture);
        _facing = Facing::Down;
        ApplyFrame(1);  // middle column = standing still
    }

    void PlayerAvatar::ApplyFrame(int column) {
        _sprite.setTextureRect(sf::IntRect(_origin.x + column * _spec.frameWidth,
                                           _origin.y + static_cast<int>(_facing) * _spec.frameHeight,
                                           _spec.frameWidth, _spec.frameHeight));
    }

    void PlayerAvatar::Update(float dt, sf::Vector2f direction, bool running, const sf::FloatRect& bounds) {
        const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length < 0.001f) {
            _animTime = 0.0f;
            ApplyFrame(1);
            return;
        }
        direction /= length;

        if (std::abs(direction.x) > std::abs(direction.y)) {
            _facing = direction.x < 0 ? Facing::Left : Facing::Right;
        } else {
            _facing = direction.y < 0 ? Facing::Up : Facing::Down;
        }

        sf::Vector2f position = _sprite.getPosition() + direction * (running ? kRunSpeed : kWalkSpeed) * dt;
        position.x = std::clamp(position.x, bounds.left, bounds.left + bounds.width - static_cast<float>(_spec.frameWidth));
        position.y = std::clamp(position.y, bounds.top, bounds.top + bounds.height - static_cast<float>(_spec.frameHeight));
        _sprite.setPosition(position);

        _animTime += dt * (running ? 1.6f : 1.0f);
        ApplyFrame(static_cast<int>(_animTime * kFramesPerSecond) % 3);
    }
}
