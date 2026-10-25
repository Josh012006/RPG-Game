#include "core/util/JsonFile.hpp"

#include <fstream>
#include <stdexcept>
#include <system_error>

#include "core/util/Log.hpp"

namespace Oathbound {

    Json ReadJsonFile(const fs::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) {
            throw std::runtime_error("cannot open '" + path.string() + "'");
        }
        try {
            return Json::parse(in);
        } catch (const Json::exception& e) {
            throw std::runtime_error("invalid JSON in '" + path.string() + "': " + e.what());
        }
    }

    fs::path BackupPathFor(const fs::path& path) {
        fs::path backup = path;
        backup += ".bak";
        return backup;
    }

    void WriteJsonFileAtomic(const fs::path& path, const Json& data, bool keepBackup) {
        try {
            if (path.has_parent_path()) {
                fs::create_directories(path.parent_path());
            }

            fs::path temp = path;
            temp += ".tmp";

            {
                std::ofstream out(temp, std::ios::binary | std::ios::trunc);
                if (!out) {
                    throw std::runtime_error("cannot open '" + temp.string() + "' for writing");
                }
                // 'replace' avoids throwing on invalid UTF-8 inside a string.
                out << data.dump(2, ' ', false, Json::error_handler_t::replace) << '\n';
                out.flush();
                if (!out) {
                    throw std::runtime_error("write failed for '" + temp.string() + "'");
                }
            }

            std::error_code ec;
            if (keepBackup && fs::exists(path, ec)) {
                fs::copy_file(path, BackupPathFor(path), fs::copy_options::overwrite_existing, ec);
                if (ec) {
                    log::Warn("could not refresh backup of '" + path.string() + "': " + ec.message());
                }
            }

            fs::rename(temp, path, ec);
            if (ec) {
                // Some platforms refuse to rename over an existing file.
                fs::remove(path, ec);
                fs::rename(temp, path);
            }
        } catch (const fs::filesystem_error& e) {
            throw std::runtime_error(std::string("file system error: ") + e.what());
        }
    }
}
