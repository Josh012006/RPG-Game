#include "states/WorldSelectionState.hpp"

#include "engine/Config.hpp"
#include "engine/UiHelpers.hpp"
#include "states/GameplayState.hpp"

namespace Oathbound {

    void WorldSelectionState::OnEnter() {
        _background.setTexture(_ctx.assets.Texture(paths::kPaperTexture));
        const sf::Font& font = _ctx.assets.Font(paths::kMainFont);
        const float cx = config::kScreenWidth / 2.0f;

        _title = ui::MakeText(font, "Choose Your World", 50, ui::kInk, {0.0f, 50.0f});
        ui::CenterAround(_title, cx);
        _instructions = ui::MakeText(font, "LEFT/RIGHT to browse  -  ENTER to select  -  ESC to go back", 20, ui::kInk, {0.0f, 120.0f});
        ui::CenterAround(_instructions, cx);
        _name = ui::MakeText(font, "", 35, ui::kInk, {100.0f, 300.0f});
        _description = ui::MakeText(font, "", 20, ui::kInk, {100.0f, 400.0f});
        _details = ui::MakeText(font, "", 20, ui::kMuted, {100.0f, 540.0f});
        _message = ui::MakeText(font, "", 22, ui::kError, {100.0f, 620.0f});
        _counter = ui::MakeText(font, "", 18, ui::kMuted, {0.0f, 850.0f});

        // Start on the world the character was last in.
        if (_ctx.session) {
            if (auto index = _ctx.content.worlds.IndexOf(_ctx.session->player.currentWorldId)) _selected = *index;
        }
        Refresh();
    }

    void WorldSelectionState::Refresh() {
        const auto& worlds = _ctx.content.worlds.All();
        if (worlds.empty()) {
            _name.setString("No world available");
            return;
        }
        const WorldDefinition& w = worlds[_selected];
        const int level = _ctx.session ? _ctx.session->player.level : 1;
        const bool unlocked = w.IsUnlockedFor(level);
        const sf::Color color = unlocked ? ui::kInk : ui::kLocked;

        _name.setString(w.name);
        _name.setFillColor(color);
        _description.setString(ui::Wrap(w.description, 80));
        _description.setFillColor(color);
        _details.setString("Difficulty: " + w.difficulty + "      Required level: " + std::to_string(w.minimumLevel) +
                           (unlocked ? "" : "      (LOCKED)"));
        _message.setString("");
        _counter.setString("World " + std::to_string(_selected + 1) + " / " + std::to_string(worlds.size()));
        ui::CenterAround(_counter, config::kScreenWidth / 2.0f);
    }

    void WorldSelectionState::Confirm() {
        const auto& worlds = _ctx.content.worlds.All();
        if (worlds.empty() || !_ctx.session) return;

        const WorldDefinition& w = worlds[_selected];
        GameSession& session = *_ctx.session;
        if (!w.IsUnlockedFor(session.player.level)) {
            _message.setString("Locked: reach level " + std::to_string(w.minimumLevel) + " to enter.");
            return;
        }

        session.player.currentWorldId = w.id;
        try {
            _ctx.saves->Save(session);
        } catch (const std::exception& e) {
            _message.setString(std::string("Could not save: ") + e.what());
            return;
        }
        _ctx.states.Replace(std::make_unique<GameplayState>(_ctx));
    }

    void WorldSelectionState::HandleEvent(const sf::Event& event) {
        if (event.type != sf::Event::KeyPressed) return;

        const std::size_t count = _ctx.content.worlds.All().size();
        switch (event.key.code) {
            case sf::Keyboard::Left:
                if (count) { _selected = (_selected + count - 1) % count; Refresh(); }
                break;
            case sf::Keyboard::Right:
                if (count) { _selected = (_selected + 1) % count; Refresh(); }
                break;
            case sf::Keyboard::Return:
                Confirm();
                break;
            case sf::Keyboard::Escape:
                _ctx.states.Pop();
                break;
            default:
                break;
        }
    }

    void WorldSelectionState::Draw() {
        _ctx.window.draw(_background);
        _ctx.window.draw(_title);
        _ctx.window.draw(_instructions);
        _ctx.window.draw(_name);
        _ctx.window.draw(_description);
        _ctx.window.draw(_details);
        _ctx.window.draw(_message);
        _ctx.window.draw(_counter);
    }
}
