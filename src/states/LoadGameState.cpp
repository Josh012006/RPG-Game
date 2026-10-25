#include "states/LoadGameState.hpp"

#include <algorithm>

#include "engine/Config.hpp"
#include "engine/Input.hpp"
#include "engine/UiHelpers.hpp"
#include "states/WorldSelectionState.hpp"

namespace Oathbound {

    void LoadGameState::OnEnter() {
        _background.setTexture(_ctx.assets.Texture(paths::kPaperTexture));
        const sf::Font& font = _ctx.assets.Font(paths::kMainFont);
        const float cx = config::kScreenWidth / 2.0f;

        _title = ui::MakeText(font, "Load Game", 50, ui::kInk, {0.0f, 50.0f});
        ui::CenterAround(_title, cx);
        _instructions = ui::MakeText(font, "UP/DOWN to choose  -  ENTER to load  -  ESC to go back", 20, ui::kInk, {0.0f, 120.0f});
        ui::CenterAround(_instructions, cx);
        _emptyText = ui::MakeText(font, "No save files found!", 30, ui::kInk, {0.0f, config::kScreenHeight / 2.0f - 50.0f});
        ui::CenterAround(_emptyText, cx);
        _statusText = ui::MakeText(font, "", 22, ui::kError, {150.0f, 880.0f});

        _saves = _ctx.saves->List();
        Rebuild();
    }

    void LoadGameState::Rebuild() {
        _rows.clear();
        if (_saves.empty()) return;

        const sf::Font& font = _ctx.assets.Font(paths::kMainFont);
        _selected = std::clamp(_selected, 0, static_cast<int>(_saves.size()) - 1);
        if (_selected < _top) _top = _selected;
        if (_selected >= _top + kVisibleRows) _top = _selected - kVisibleRows + 1;

        const int last = std::min<int>(static_cast<int>(_saves.size()), _top + kVisibleRows);
        for (int i = _top; i < last; ++i) {
            const SaveSummary& s = _saves[static_cast<std::size_t>(i)];
            std::string label;
            sf::Color color = (i == _selected) ? ui::kInk : ui::kMuted;

            if (s.valid) {
                label = s.playerName + "   Lv " + std::to_string(s.level) + "   " + s.worldName + "   " + ui::PrettyDate(s.updatedAt);
            } else {
                label = s.id + "   [unreadable]";
                color = ui::kError;
            }
            if (i == _selected) label = "> " + label;

            _rows.push_back(ui::MakeText(font, label, 26, color, {150.0f, 240.0f + static_cast<float>(i - _top) * 56.0f}));
        }
    }

    void LoadGameState::LoadSelected() {
        if (_saves.empty()) return;
        const SaveSummary& summary = _saves[static_cast<std::size_t>(_selected)];
        if (!summary.valid) {
            _statusText.setString("This save cannot be read: " + summary.error);
            return;
        }
        try {
            _ctx.session = std::make_unique<GameSession>(_ctx.saves->Load(summary.id));
        } catch (const std::exception& e) {
            _statusText.setString(std::string("Could not load: ") + e.what());
            return;
        }
        _ctx.states.Replace(std::make_unique<WorldSelectionState>(_ctx));
    }

    void LoadGameState::HandleEvent(const sf::Event& event) {
        if (event.type != sf::Event::KeyPressed) return;

        const int count = static_cast<int>(_saves.size());
        switch (event.key.code) {
            case sf::Keyboard::Up:
                if (count > 0) { _selected = (_selected + count - 1) % count; _statusText.setString(""); Rebuild(); }
                break;
            case sf::Keyboard::Down:
                if (count > 0) { _selected = (_selected + 1) % count; _statusText.setString(""); Rebuild(); }
                break;
            case sf::Keyboard::Return:
                LoadSelected();
                break;
            case sf::Keyboard::Escape:
                _ctx.states.Pop();
                break;
            default:
                break;
        }
    }

    void LoadGameState::Draw() {
        _ctx.window.draw(_background);
        _ctx.window.draw(_title);
        _ctx.window.draw(_instructions);
        if (_saves.empty()) {
            _ctx.window.draw(_emptyText);
        } else {
            for (const sf::Text& row : _rows) _ctx.window.draw(row);
        }
        _ctx.window.draw(_statusText);
    }
}
