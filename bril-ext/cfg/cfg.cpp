#include "cfg.h"
#include "../form-blocks/form-block.h"
#include <iostream>
#include <unordered_map>

cfg_map create_cfg(json &f) {
  cfg_map map;

  auto blocks = get_named_blocks(f);

  // std::cerr<<"Blocks\n";
  // std::cerr<<blocks.dump(2);
  // std::cerr<<"\n";
  // std::cerr<<"\n";

  std::string prev_block = "";

  for(auto block: blocks) {
    auto key = block["name"];
    auto value = block["insts"];
    
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

  // for (auto it : blocks.items()) {
  //   auto key = (*it).key();
  //   auto value = (*it).value();
    
  //   std::cerr<<"Key: "<<key<<"\n";

  //   map[key] = std::vector<std::string>();

  //   if (prev_block != "") {
  //     map[prev_block].push_back((*it).key());
  //     prev_block = "";
  //   }

  //   for (auto inst : (*it).value()) {
  //     if (inst["op"] == JMP) {
  //       map[key].push_back(inst["labels"][0]);
  //       prev_block = "";
  //     } else if (inst["op"] == BR) {
  //       map[key].push_back(inst["labels"][0]);
  //       map[key].push_back(inst["labels"][1]);
  //       prev_block = "";
  //     } else if (inst["op"] == RET) {
  //       prev_block = "";
  //     } else {
  //       prev_block = key;
  //     }
  //   }
  // }

  return map;
}
