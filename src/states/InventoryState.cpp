#include "states/InventoryState.hpp"

#include <algorithm>

#include "core/util/Log.hpp"
#include "engine/Config.hpp"
#include "engine/UiHelpers.hpp"

namespace Oathbound {

    void InventoryState::OnEnter() {
        if (!_ctx.session) _ctx.states.Pop();
    }

    void InventoryState::OnExit() {
        // Persist whatever the player did here (use, drop, sort, rearrange).
        if (!_ctx.session) return;
        try {
            _ctx.saves->Save(*_ctx.session);
        } catch (const std::exception& e) {
            log::Error(std::string("inventory: could not save: ") + e.what());
        }
    }

    void InventoryState::Say(const std::string& message, sf::Color color) {
        _message = message;
        _messageColor = color;
    }

    void InventoryState::MoveCursor(int dx, int dy) {
        const std::size_t capacity = Inv().Capacity();
        if (capacity == 0) return;

        long long col = static_cast<long long>(_cursor % kColumns) + dx;
        long long row = static_cast<long long>(_cursor / kColumns) + dy;
        const long long rows = static_cast<long long>((capacity + kColumns - 1) / kColumns);
        col = std::clamp<long long>(col, 0, static_cast<long long>(kColumns) - 1);
        row = std::clamp<long long>(row, 0, rows - 1);

        _cursor = std::min<std::size_t>(static_cast<std::size_t>(row) * kColumns + static_cast<std::size_t>(col), capacity - 1);

        const std::size_t cursorRow = _cursor / kColumns;
        if (cursorRow < _firstRow) _firstRow = cursorRow;
        if (cursorRow >= _firstRow + kVisibleRows) _firstRow = cursorRow - kVisibleRows + 1;
    }

    void InventoryState::HandleEvent(const sf::Event& event) {
        if (event.type != sf::Event::KeyPressed || !_ctx.session) return;

        const ItemDatabase& items = _ctx.content.items;
        switch (event.key.code) {
            case sf::Keyboard::Left:  MoveCursor(-1, 0); break;
            case sf::Keyboard::Right: MoveCursor(1, 0); break;
            case sf::Keyboard::Up:    MoveCursor(0, -1); break;
            case sf::Keyboard::Down:  MoveCursor(0, 1); break;

            case sf::Keyboard::Return:
            case sf::Keyboard::E: {
                const UseResult r = UseItem(_ctx.session->player, _cursor, items, _ctx.content.behaviors);
                const bool good = r.outcome == UseOutcome::Consumed || r.outcome == UseOutcome::Kept;
                Say(r.message, good ? ui::kGood : ui::kError);
                break;
            }
            case sf::Keyboard::X: {
                const ItemStack* stack = Inv().At(_cursor);
                if (!stack) break;
                const ItemDefinition* def = items.Find(stack->id);
                const std::string name = def ? def->name : stack->id;
                if (Inv().RemoveFromSlot(_cursor, 1)) Say("Dropped 1 x " + name + ".", ui::kMuted);
                break;
            }
            case sf::Keyboard::Space:
                if (!_grabbed) {
                    if (Inv().At(_cursor)) _grabbed = _cursor;
                } else {
                    if (*_grabbed != _cursor) Inv().Move(*_grabbed, _cursor);
                    _grabbed.reset();
                }
                break;
            case sf::Keyboard::S:
                Inv().Sort();
                _grabbed.reset();
                Say("Inventory sorted.", ui::kMuted);
                break;
            case sf::Keyboard::Escape:
            case sf::Keyboard::I:
                _ctx.states.Pop();
                break;
            default:
                break;
        }
    }

    void InventoryState::Draw() {
        if (!_ctx.session) return;
        sf::RenderWindow& win = _ctx.window;
        const Inventory& inv = _ctx.session->player.inventory;
        const sf::Font& font = _ctx.assets.Font(paths::kMainFont);
        const float W = static_cast<float>(config::kScreenWidth);
        const float H = static_cast<float>(config::kScreenHeight);

        sf::RectangleShape dim({W, H});
        dim.setFillColor(sf::Color(0, 0, 0, 170));
        win.draw(dim);

        sf::RectangleShape panel({1600.0f, 798.0f});
        panel.setPosition(96.0f, 96.0f);
        panel.setFillColor(sf::Color(236, 222, 190));
        panel.setOutlineColor(ui::kInk);
        panel.setOutlineThickness(4.0f);
        win.draw(panel);

        win.draw(ui::MakeText(font, "Inventory   " + std::to_string(inv.UsedSlots()) + " / " + std::to_string(inv.Capacity()),
                              36, ui::kInk, {kGridX, 120.0f}));

        // ---- grid ----
        const std::size_t capacity = inv.Capacity();
        for (std::size_t i = _firstRow * kColumns; i < capacity && i < (_firstRow + kVisibleRows) * kColumns; ++i) {
            const float x = kGridX + static_cast<float>(i % kColumns) * (kSlotSize + kGap);
            const float y = kGridY + static_cast<float>(i / kColumns - _firstRow) * (kSlotSize + kGap);
            const ItemStack* stack = inv.At(i);

            sf::RectangleShape slot({kSlotSize, kSlotSize});
            slot.setPosition(x, y);
            slot.setFillColor(stack ? sf::Color(205, 180, 130) : sf::Color(222, 205, 170));
            slot.setOutlineThickness(i == _cursor || (_grabbed && *_grabbed == i) ? 4.0f : 1.0f);
            slot.setOutlineColor(_grabbed && *_grabbed == i ? sf::Color(40, 80, 180)
                                 : i == _cursor            ? ui::kInk
                                                           : sf::Color(150, 125, 90));
            win.draw(slot);

            if (stack) {
                const ItemDefinition* def = _ctx.content.items.Find(stack->id);
                win.draw(ui::MakeText(font, ui::Wrap(def ? def->name : "? " + stack->id, 10), 15, ui::kInk, {x + 6.0f, y + 6.0f}));
                if (stack->quantity > 1) {
                    sf::Text qty = ui::MakeText(font, "x" + std::to_string(stack->quantity), 20, ui::kInk, {0.0f, 0.0f});
                    const sf::FloatRect b = qty.getLocalBounds();
                    qty.setPosition(x + kSlotSize - b.width - 8.0f, y + kSlotSize - 30.0f);
                    win.draw(qty);
                }
            }
        }

        // ---- details of the selected slot ----
        const float dx = kGridX + static_cast<float>(kColumns) * (kSlotSize + kGap) + 60.0f;
        if (const ItemStack* stack = inv.At(_cursor)) {
            const ItemDefinition* def = _ctx.content.items.Find(stack->id);
            win.draw(ui::MakeText(font, def ? def->name : stack->id, 34, ui::kInk, {dx, 190.0f}));

            if (def) {
                win.draw(ui::MakeText(font, "Category: " + def->category + "      Value: " + std::to_string(def->value), 20, ui::kMuted, {dx, 245.0f}));
                win.draw(ui::MakeText(font, "Quantity: " + std::to_string(stack->quantity) + " / " + std::to_string(def->maxStack), 20, ui::kMuted, {dx, 275.0f}));
                win.draw(ui::MakeText(font, ui::Wrap(def->description, 52), 22, ui::kInk, {dx, 325.0f}));
            } else {
                win.draw(ui::MakeText(font, "This item no longer exists in the game data.\nIt is kept safely in your save.", 20, ui::kError, {dx, 245.0f}));
            }
            if (!IsEmptyState(stack->state)) {
                win.draw(ui::MakeText(font, "Condition: " + ui::Wrap(stack->state.dump(), 50), 18, ui::kMuted, {dx, 470.0f}));
            }
        } else {
            win.draw(ui::MakeText(font, "Empty slot", 28, ui::kMuted, {dx, 190.0f}));
        }

        if (_grabbed) {
            win.draw(ui::MakeText(font, "Item picked up: choose a slot and press SPACE to put it down.", 20, sf::Color(40, 80, 180), {kGridX, 790.0f}));
        }
        win.draw(ui::MakeText(font, _message, 24, _messageColor, {kGridX, 825.0f}));
        win.draw(ui::MakeText(font, "Arrows: move   Enter: use   X: drop one   Space: pick up / put down   S: sort   Esc: close",
                              18, ui::kMuted, {kGridX, 858.0f}));
    }
}
