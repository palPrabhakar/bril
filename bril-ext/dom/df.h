#pragma once

#include "../json.hpp"
#include <unordered_map>
#include <vector>

using json = nlohmann::json;
using df_map = std::unordered_map<std::string, std::vector<std::string>>;
using dom_tree = std::unordered_map<std::string, std::vector<std::string>>;
using idom_map = std::unordered_map<std::string, std::string>;

// TODO:
// refactor dominator API
// the functions shouldn't compute cfg, blocks and other stuff
df_map find_dominance_frontier(json &f);
dom_tree create_dominator_tree(json &f);
