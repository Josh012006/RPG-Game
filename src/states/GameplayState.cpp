#include "states/GameplayState.hpp"

#include <filesystem>

#include "core/util/Log.hpp"
#include "engine/Config.hpp"
#include "engine/Input.hpp"
#include "engine/UiHelpers.hpp"
#include "states/InventoryState.hpp"

namespace Oathbound {

    void GameplayState::OnEnter() {
        const float w = static_cast<float>(config::kScreenWidth);
        const float h = static_cast<float>(config::kScreenHeight);

        _fallbackBackground.setSize({w, h});
        _fallbackBackground.setFillColor(sf::Color(24, 34, 28));

        if (_ctx.session) {
            const WorldDefinition* world = _ctx.content.worlds.Find(_ctx.session->player.currentWorldId);
            if (world && !world->background.empty() && std::filesystem::exists(world->background)) {
                const sf::Texture& tex = _ctx.assets.Texture(world->background);
                _background.setTexture(tex);
                _background.setScale(w / static_cast<float>(tex.getSize().x), h / static_cast<float>(tex.getSize().y));
                _hasBackground = true;
            }
        }

        _avatar.Setup(_ctx.assets.Texture(paths::kHeroSheet), AvatarSpec{});
        _avatar.SetPosition({w / 2.0f - 24.0f, h / 2.0f - 24.0f});

        const sf::Font& font = _ctx.assets.Font(paths::kMainFont);
        _hud = ui::MakeText(font, "", 24, sf::Color::White, {20.0f, 14.0f});
        _hud.setOutlineColor(sf::Color::Black);
        _hud.setOutlineThickness(2.0f);
        _hint = ui::MakeText(font, "I: inventory   F5: save   Esc: menu   Shift: run", 18, sf::Color(220, 220, 220), {20.0f, h - 34.0f});
        _hint.setOutlineColor(sf::Color::Black);
        _hint.setOutlineThickness(1.5f);
        _toast = ui::MakeText(font, "", 26, sf::Color::White, {20.0f, 60.0f});
        _toast.setOutlineColor(sf::Color::Black);
        _toast.setOutlineThickness(2.0f);

        _ctx.fade.Start(true, 0.6f);
    }

    void GameplayState::Toast(const std::string& message, sf::Color color) {
        _toast.setString(message);
        _toast.setFillColor(color);
        _toastTimer = 3.0f;
    }

    void GameplayState::Save(bool announce) {
        if (!_ctx.session) return;
        try {
            _ctx.saves->Save(*_ctx.session);
            if (announce) Toast("Game saved.", sf::Color(150, 230, 150));
        } catch (const std::exception& e) {
            log::Error(e.what());
            Toast(std::string("Save failed: ") + e.what(), sf::Color(255, 140, 140));
        }
    }

    void GameplayState::HandleEvent(const sf::Event& event) {
        if (event.type != sf::Event::KeyPressed || !_ctx.session) return;

        switch (event.key.code) {
            case sf::Keyboard::I:
                _ctx.states.Push(std::make_unique<InventoryState>(_ctx));
                break;
            case sf::Keyboard::F5:
                Save(true);
                break;
            case sf::Keyboard::Escape:
                Save(false);
                _ctx.states.Pop();  // back to the main menu
                break;
            case sf::Keyboard::F2:
                if (config::kEnableDebugKeys && _ctx.content.items.Size() > 0) {
                    std::uniform_int_distribution<std::size_t> pick(0, _ctx.content.items.Size() - 1);
                    const ItemDefinition& def = _ctx.content.items.All()[pick(_rng)];
                    if (_ctx.session->player.inventory.Add(def.id, 1) == 0) {
                        Toast("[debug] +1 " + def.name, sf::Color(255, 230, 150));
                    } else {
                        Toast("[debug] inventory full", sf::Color(255, 140, 140));
                    }
                }
                break;
            default:
                break;
        }
    }

    void GameplayState::Update(float dt) {
        if (_toastTimer > 0.0f) _toastTimer -= dt;

        sf::Vector2f direction(0.0f, 0.0f);
        bool running = false;
        if (_ctx.window.hasFocus()) {  // sf::Keyboard polls globally, even when another window has focus
            using K = sf::Keyboard;
            if (K::isKeyPressed(K::Left)  || K::isKeyPressed(K::A) || K::isKeyPressed(K::Q)) direction.x -= 1.0f;
            if (K::isKeyPressed(K::Right) || K::isKeyPressed(K::D)) direction.x += 1.0f;
            if (K::isKeyPressed(K::Up)    || K::isKeyPressed(K::W) || K::isKeyPressed(K::Z)) direction.y -= 1.0f;
            if (K::isKeyPressed(K::Down)  || K::isKeyPressed(K::S)) direction.y += 1.0f;
            running = K::isKeyPressed(K::LShift) || K::isKeyPressed(K::RShift);
        }

        _avatar.Update(dt, direction, running,
                       sf::FloatRect(0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)));
        RefreshHud();
    }

    void GameplayState::RefreshHud() {
        if (!_ctx.session) return;
        const PlayerProfile& p = _ctx.session->player;
        const WorldDefinition* world = _ctx.content.worlds.Find(p.currentWorldId);
        _hud.setString(p.name + "   Lv " + std::to_string(p.level) + "   HP " + std::to_string(p.health) + "/" +
                       std::to_string(p.maxHealth) + "   " + (world ? world->name : "Unknown world"));
    }

    void GameplayState::Draw() {
        if (_hasBackground) _ctx.window.draw(_background);
        else _ctx.window.draw(_fallbackBackground);

        _avatar.Draw(_ctx.window);
        _ctx.window.draw(_hud);
        _ctx.window.draw(_hint);
        if (_toastTimer > 0.0f) _ctx.window.draw(_toast);
    }
}
