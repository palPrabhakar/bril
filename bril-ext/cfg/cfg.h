#ifndef __CFG_H
#define __CFG_H

#include "../json.hpp"
using json = nlohmann::json;

using cfg_map = std::unordered_map<std::string, std::vector<std::string>>;

cfg_map create_cfg(json &f);

#endif