#pragma once

#include <filesystem>

#include "core/util/Json.hpp"

namespace Oathbound {
    namespace fs = std::filesystem;

    // Reads and parses a JSON file. Throws std::runtime_error (with the path) on any failure.
    Json ReadJsonFile(const fs::path& path);

    // "<file>.bak": the previous good version of a file written by WriteJsonFileAtomic.
    fs::path BackupPathFor(const fs::path& path);

    // Crash-safe write: the data goes to "<file>.tmp" first, the previous version is kept as
    // "<file>.bak", then the temp file is renamed over the target. A crash or a full disk in the
    // middle of a save can therefore never leave a half-written file in place of a good one.
    // Throws std::runtime_error on failure.
    void WriteJsonFileAtomic(const fs::path& path, const Json& data, bool keepBackup);
}
