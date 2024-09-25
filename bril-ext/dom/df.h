#pragma once

#include "../json.hpp"
#include <unordered_map>
#include <vector>

using json = nlohmann::json;
using df_map = std::unordered_map<std::string, std::vector<std::string>>;

df_map find_dominance_frontier(json &f);

