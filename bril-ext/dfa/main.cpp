#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "../json.hpp"
#include "../cfg/cfg.h"
#include "../form-blocks/form-block.h"

// Do data flow analysis
void do_dfa() {
  json program = json::parse(stdin);

  for(auto &f: program["functions"]) {
    auto blocks = get_named_blocks(f); 
    std::cerr<<blocks.dump(2);
    auto cfg_map = create_cfg(blocks);
  }

  // std::cerr<<program.dump(2);
}

int main() {
  // Reaching definitions
  do_dfa();

  return 0;
}
