#include "cfg.h"
#include "../form-blocks/form-block.h"
#include <iostream>
#include <unordered_map>

cfg_map get_predecessor_map(cfg_map map) {
  cfg_map pred;

  for(auto [key, value]: map) {
    for(auto val: value) {
      pred[val].push_back(key);
    }
  }

  return pred;
}

cfg_map create_cfg(json &blocks) {
  cfg_map map;

  std::string prev_block = "";

  for (auto block : blocks) {
    auto key = block["name"];
    auto value = block["instrs"];

    map[key] = std::vector<std::string>();

    if (prev_block != "") {
      map[prev_block].push_back(key);
      prev_block = "";
    }

    for (auto inst : value) {
      if (inst["op"] == JMP) {
        map[key].push_back(inst["labels"][0]);
        prev_block = "";
      } else if (inst["op"] == BR) {
        map[key].push_back(inst["labels"][0]);
        map[key].push_back(inst["labels"][1]);
        prev_block = "";
      } else if (inst["op"] == RET) {
        prev_block = "";
      } else {
        prev_block = key;
      }
    }
  }

  return map;
}

void print_cfg(cfg_map map, std::string fn_name) {
  std::cerr << fn_name << ": {\n";
  for (auto [key, value] : map) {
    std::cerr << "  " << key << ": ";
    for (auto i = 0; i < value.size(); ++i) {
      if (i != value.size() - 1) {
        std::cerr <<value[i] << ", ";
      } else {
        std::cerr <<value[i] << ";";
      }
    }
    std::cerr << "\n";
  }
  std::cerr << "}\n";
}
