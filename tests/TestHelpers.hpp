#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>

#include "core/items/ItemDatabase.hpp"

namespace fs = std::filesystem;

// A small catalog independent from data/items.json, so inventory tests do not break when the
// game designer rebalances the real items.
inline Oathbound::ItemDatabase MakeTestItems() {
    using Oathbound::Json;
    Json items = Json::parse(R"({"items":[
        {"id":"potion","name":"Potion","category":"consumable","maxStack":10,"aliases":["old_potion"],"properties":{"heal":25}},
        {"id":"sword","name":"Sword","category":"weapon","maxStack":1,"defaultState":{"durability":100}},
        {"id":"herb","name":"Herb","category":"material","maxStack":99},
        {"id":"key","name":"Key","category":"quest","maxStack":1}
    ]})");
    Oathbound::ItemDatabase db;
    db.LoadFromJson(items);
    return db;
}

// RAII temporary directory.
struct TempDir {
    fs::path path;
    TempDir() {
        static std::atomic<int> counter{0};
        path = fs::temp_directory_path() /
               ("oathbound_test_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
                "_" + std::to_string(counter++));
        fs::create_directories(path);
    }
    ~TempDir() { std::error_code ec; fs::remove_all(path, ec); }
    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;
};

inline void WriteText(const fs::path& file, const std::string& text) {
    fs::create_directories(file.parent_path());
    std::ofstream(file, std::ios::binary) << text;
}
