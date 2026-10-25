#pragma once

#include <string>

#include <SFML/Graphics.hpp>

// Small shared UI bits: the "ink" colour and text construction were copy-pasted in every state.
namespace Oathbound::ui {

    inline const sf::Color kInk(58, 18, 16);
    inline const sf::Color kMuted(100, 100, 100);
    inline const sf::Color kLocked(150, 150, 150);
    inline const sf::Color kError(150, 30, 30);
    inline const sf::Color kGood(20, 110, 40);

    inline sf::Text MakeText(const sf::Font& font, const std::string& text, unsigned size,
                             sf::Color color, sf::Vector2f position) {
        sf::Text t(text, font, size);
        t.setFillColor(color);
        t.setPosition(position);
        return t;
    }

    // Centres a text horizontally around `centerX` (keeps its y).
    inline void CenterAround(sf::Text& text, float centerX) {
        const sf::FloatRect b = text.getLocalBounds();
        text.setOrigin(b.left + b.width / 2.0f, 0.0f);
        text.setPosition(centerX, text.getPosition().y);
    }

    // "2026-06-24T22:57:00Z" -> "2026-06-24 22:57"
    inline std::string PrettyDate(const std::string& iso) {
        if (iso.size() < 16) return iso;
        std::string out = iso.substr(0, 16);
        out[10] = ' ';
        return out;
    }

    // Greedy word wrap on a character budget (the font is not monospaced, so this is approximate).
    inline std::string Wrap(const std::string& text, std::size_t maxChars) {
        std::string out, line;
        std::size_t pos = 0;
        while (pos <= text.size()) {
            std::size_t next = text.find(' ', pos);
            if (next == std::string::npos) next = text.size();
            const std::string word = text.substr(pos, next - pos);
            if (!line.empty() && line.size() + 1 + word.size() > maxChars) {
                out += line + '\n';
                line = word;
            } else {
                line += (line.empty() ? "" : " ") + word;
            }
            pos = next + 1;
        }
        return out + line;
    }
}
