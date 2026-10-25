#pragma once

#include <string>
#include <vector>

#include "core/content/GameContent.hpp"
#include "core/save/GameSession.hpp"

namespace Oathbound {

    // What the "Load game" screen needs, without building a whole GameSession per file.
    struct SaveSummary {
        std::string id;
        std::string playerName;
        int level = 1;
        std::string worldName;
        std::string updatedAt;
        bool valid = true;
        std::string error;  // filled when !valid
    };

    // The only class that touches save files. One file per save: <dir>/<id>.json
    // (+ <id>.json.bak = previous good version, kept automatically).
    class SaveManager {
    public:
        SaveManager(fs::path saveDirectory, const GameContent& content);

        // Newest first. Unreadable files are listed with valid=false instead of being hidden.
        std::vector<SaveSummary> List() const;

        // Creates the save file for a brand-new character (starting items from new_game.json).
        GameSession CreateNew(const std::string& playerName);

        // Throws SaveError. If the main file is corrupt but the .bak is fine, the backup is
        // restored automatically (the corrupt file is kept as <id>.json.corrupt).
        GameSession Load(const std::string& id);

        // Atomic write. Throws SaveError.
        void Save(GameSession& session);

        bool Delete(const std::string& id);

        // Imports the prototype's "data/saveData*.json" files into the new save folder, then
        // renames each imported file to "*.migrated". Returns how many were imported.
        int ImportLegacySaves(const fs::path& legacyDirectory);

        fs::path PathFor(const std::string& id) const;

    private:
        Json ReadSaveJson(const std::string& id, bool* usedBackup) const;
        std::string NextId() const;

        fs::path _directory;
        const GameContent& _content;
    };
}
