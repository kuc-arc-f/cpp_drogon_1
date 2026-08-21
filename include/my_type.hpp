#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ─────────────────────────────────────────
//  Data model
// ─────────────────────────────────────────
struct Todo {
    int         id;
    std::string title;
    bool        done;
    std::string created_at;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Todo, id, title, done, created_at)


