#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

#include "engine/Application.hpp"

namespace fs = std::filesystem;

// Assets and data are referenced by relative paths ("media/...", "data/..."), so the process
// must run from the game root. This lets the game start correctly whether it is launched from
// the root, from bin/, or by double-clicking the executable.
static void LocateGameRoot(const char* argv0) {
    std::vector<fs::path> candidates{fs::current_path()};
    if (argv0 && *argv0) {
        std::error_code ec;
        const fs::path exeDir = fs::weakly_canonical(fs::absolute(argv0), ec).parent_path();
        if (!ec) {
            candidates.push_back(exeDir);
            candidates.push_back(exeDir.parent_path());
            candidates.push_back(exeDir.parent_path().parent_path());
        }
    }
    for (const fs::path& dir : candidates) {
        if (fs::exists(dir / "data" / "items.json")) {
            fs::current_path(dir);
            return;
        }
    }
}

int main(int, char* argv[]) {
    try {
        LocateGameRoot(argv[0]);
        Oathbound::Application app;
        return app.Run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
