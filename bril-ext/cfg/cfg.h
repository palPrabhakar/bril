#ifndef __CFG_H
#define __CFG_H

#include "../json.hpp"
using json = nlohmann::json;

using cfg_map = std::unordered_map<std::string, std::vector<std::string>>;

cfg_map create_cfg(json &blocks);

cfg_map get_predecessor_map(cfg_map map);

void print_cfg(cfg_map map, std::string fn_name);

#endif
