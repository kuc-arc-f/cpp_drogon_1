#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Todo {
    int id;
    std::string title;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Todo, id, title)


