#include "core/save/SaveManager.hpp"

#include <algorithm>
#include <cctype>
#include <system_error>

#include "core/player/PlayerProfile.hpp"
#include "core/save/SaveCodec.hpp"
#include "core/save/SaveError.hpp"
#include "core/util/Log.hpp"
#include "core/util/Time.hpp"

namespace Oathbound {

    namespace {
        // Ids come from file names; refusing anything but [A-Za-z0-9_-] rules out path tricks.
        bool IsValidSaveId(const std::string& id) {
            return !id.empty() && id.size() <= 64 &&
                   std::all_of(id.begin(), id.end(), [](unsigned char c) {
                       return std::isalnum(c) || c == '_' || c == '-';
                   });
        }
    }

    SaveManager::SaveManager(fs::path saveDirectory, const GameContent& content)
        : _directory(std::move(saveDirectory)), _content(content) {}

    fs::path SaveManager::PathFor(const std::string& id) const {
        return _directory / (id + ".json");
    }

    std::string SaveManager::NextId() const {
        for (int n = 1;; ++n) {
            const std::string id = "save_" + std::to_string(n);
            std::error_code ec;
            if (!fs::exists(PathFor(id), ec) && !fs::exists(BackupPathFor(PathFor(id)), ec)) return id;
        }
    }

    Json SaveManager::ReadSaveJson(const std::string& id, bool* usedBackup) const {
        if (usedBackup) *usedBackup = false;
        const fs::path main = PathFor(id);
        const fs::path backup = BackupPathFor(main);

        try {
            return MigrateSaveData(ReadJsonFile(main), _content.worlds);
        } catch (const SaveTooNewError&) {
            throw;  // never fall back to an older backup
        } catch (const std::exception& primary) {
            std::error_code ec;
            if (fs::exists(backup, ec)) {
                try {
                    Json recovered = MigrateSaveData(ReadJsonFile(backup), _content.worlds);
                    log::Warn("save '" + id + "' is unreadable (" + primary.what() + "), using its backup");
                    if (usedBackup) *usedBackup = true;
                    return recovered;
                } catch (const std::exception&) {
                    // backup is bad too: report the original problem below
                }
            }
            throw SaveError("cannot read save '" + id + "': " + primary.what());
        }
    }

    std::vector<SaveSummary> SaveManager::List() const {
        std::vector<SaveSummary> result;
        std::error_code ec;
        if (!fs::is_directory(_directory, ec)) return result;

        for (const auto& entry : fs::directory_iterator(_directory, ec)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".json") continue;
            const std::string id = entry.path().stem().string();
            if (!IsValidSaveId(id)) continue;

            SaveSummary summary;
            summary.id = id;
            try {
                const Json data = ReadSaveJson(id, nullptr);
                const Json& player = data.at("player");
                summary.playerName = player.value("name", "?");
                summary.level = player.value("level", 1);

                const std::string worldId = player.value("currentWorldId", "");
                if (const WorldDefinition* w = _content.worlds.Find(worldId)) {
                    summary.worldName = w->name;
                } else {
                    summary.worldName = worldId.empty() ? "No world selected" : worldId;
                }
                if (auto meta = data.find("meta"); meta != data.end()) {
                    summary.updatedAt = meta->value("updatedAt", "");
                }
            } catch (const std::exception& e) {
                summary.valid = false;
                summary.error = e.what();
            }
            result.push_back(std::move(summary));
        }

        std::sort(result.begin(), result.end(), [](const SaveSummary& a, const SaveSummary& b) {
            return a.updatedAt != b.updatedAt ? a.updatedAt > b.updatedAt : a.id < b.id;
        });
        return result;
    }

    GameSession SaveManager::CreateNew(const std::string& playerName) {
        const std::string name = player_name::Normalize(playerName);
        if (!player_name::IsValid(name)) {
            throw SaveError("invalid character name");
        }

        const NewGameTemplate& tpl = _content.newGame;
        PlayerProfile profile(_content.items, name, tpl.inventoryCapacity);
        profile.level = tpl.level;
        for (const StartingItem& item : tpl.startingItems) {
            if (profile.inventory.Add(item.id, item.quantity) > 0) {
                log::Warn("starting inventory is full, could not give '" + item.id + "'");
            }
        }

        GameSession session(std::move(profile));
        session.saveId = NextId();
        session.createdAt = NowIso8601Utc();
        Save(session);
        return session;
    }

    GameSession SaveManager::Load(const std::string& id) {
        if (!IsValidSaveId(id)) throw SaveError("invalid save id '" + id + "'");

        bool usedBackup = false;
        const Json data = ReadSaveJson(id, &usedBackup);
        GameSession session = DecodeSession(data, _content);
        session.saveId = id;

        if (usedBackup) {
            // Put things back in order so the next Save() does not overwrite the good backup
            // with the corrupt file.
            std::error_code ec;
            const fs::path main = PathFor(id);
            fs::path corrupt = main;
            corrupt += ".corrupt";
            fs::rename(main, corrupt, ec);
            fs::copy_file(BackupPathFor(main), main, fs::copy_options::overwrite_existing, ec);
        }
        return session;
    }

    void SaveManager::Save(GameSession& session) {
        if (!IsValidSaveId(session.saveId)) throw SaveError("session has no valid save id");

        session.updatedAt = NowIso8601Utc();
        if (session.createdAt.empty()) session.createdAt = session.updatedAt;

        try {
            WriteJsonFileAtomic(PathFor(session.saveId), EncodeSession(session), /*keepBackup=*/true);
        } catch (const std::exception& e) {
            throw SaveError(std::string("cannot write save: ") + e.what());
        }
    }

    bool SaveManager::Delete(const std::string& id) {
        if (!IsValidSaveId(id)) return false;
        std::error_code ec;
        const bool removed = fs::remove(PathFor(id), ec);
        fs::remove(BackupPathFor(PathFor(id)), ec);
        return removed;
    }

    int SaveManager::ImportLegacySaves(const fs::path& legacyDirectory) {
        std::error_code ec;
        if (!fs::is_directory(legacyDirectory, ec)) return 0;

        std::vector<fs::path> files;
        for (const auto& entry : fs::directory_iterator(legacyDirectory, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json" &&
                entry.path().filename().string().rfind("saveData", 0) == 0) {
                files.push_back(entry.path());
            }
        }
        std::sort(files.begin(), files.end());

        int imported = 0;
        for (const fs::path& file : files) {
            try {
                const Json data = MigrateSaveData(ReadJsonFile(file), _content.worlds);
                GameSession session = DecodeSession(data, _content);
                session.saveId = NextId();
                Save(session);

                fs::path done = file;
                done += ".migrated";
                fs::rename(file, done);
                log::Info("imported legacy save " + file.filename().string() + " as " + session.saveId);
                ++imported;
            } catch (const std::exception& e) {
                log::Warn("skipped legacy save " + file.filename().string() + ": " + e.what());
            }
        }
        return imported;
    }
}
