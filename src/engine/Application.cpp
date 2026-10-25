#include "engine/Application.hpp"

#include <algorithm>

#include "core/util/Log.hpp"
#include "engine/Config.hpp"
#include "states/SplashState.hpp"

namespace Oathbound {

    Application::Application() {
        _ctx.content = GameContent::LoadFromDirectory(paths::kDataDir);
        _ctx.saves = std::make_unique<SaveManager>(paths::kSaveDir, _ctx.content);

        // One-shot upgrade of the prototype's data/saveData*.json files.
        if (const int imported = _ctx.saves->ImportLegacySaves(paths::kDataDir); imported > 0) {
            log::Info("imported " + std::to_string(imported) + " save(s) from the previous version");
        }

        _ctx.window.create(sf::VideoMode(config::kScreenWidth, config::kScreenHeight), config::kGameName,
                           sf::Style::Close | sf::Style::Titlebar);
        _ctx.window.setFramerateLimit(60);  // the old loop spun a CPU core at 100%
        SetupWindowDecorations();

        _ctx.states.Push(std::make_unique<SplashState>(_ctx));
    }

    void Application::SetupWindowDecorations() {
        sf::Image icon;
        if (icon.loadFromFile(paths::kGameIcon)) {
            _ctx.window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        } else {
            log::Warn(std::string("cannot load icon ") + paths::kGameIcon);
        }

        sf::Image cursorImage;
        if (cursorImage.loadFromFile(paths::kCursor)) {
            // Real image size instead of a hard-coded 50x50.
            if (_cursor.loadFromPixels(cursorImage.getPixelsPtr(), cursorImage.getSize(), sf::Vector2u(14, 6))) {
                _ctx.window.setMouseCursor(_cursor);
            }
        } else {
            log::Warn(std::string("cannot load cursor ") + paths::kCursor);
        }
    }

    int Application::Run() {
        sf::Clock clock;
        float accumulator = 0.0f;

        while (_ctx.window.isOpen()) {
            _ctx.states.ApplyPending();
            if (_ctx.states.Empty()) break;

            // Cap the frame time so a long stall (window drag...) does not cause a burst of updates.
            accumulator += std::min(clock.restart().asSeconds(), 0.25f);

            // Events: polled ONCE per frame and delivered one by one to the active state.
            sf::Event event;
            while (_ctx.window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _ctx.window.close();
                    break;
                }
                _ctx.states.Active().HandleEvent(event);
            }
            if (!_ctx.window.isOpen()) break;

            // Logic: fixed time step.
            while (accumulator >= config::kTimeStep) {
                _ctx.states.Active().Update(config::kTimeStep);
                _ctx.fade.Update(config::kTimeStep);
                if (_ctx.session) _ctx.session->playTimeSeconds += config::kTimeStep;
                accumulator -= config::kTimeStep;
            }

            _ctx.window.clear(sf::Color::Black);
            _ctx.states.DrawAll();
            _ctx.fade.Draw(_ctx.window);
            _ctx.window.display();
        }

        SaveOnExit();
        return 0;
    }

    void Application::SaveOnExit() {
        if (!_ctx.session || !_ctx.saves) return;
        try {
            _ctx.saves->Save(*_ctx.session);
        } catch (const std::exception& e) {
            log::Error(std::string("could not save on exit: ") + e.what());
        }
    }
}
