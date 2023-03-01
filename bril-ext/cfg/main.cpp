#include <iostream>
#include "../json.hpp"
#include "../form-blocks/form-block.h"
#include "cfg.h"

using json = nlohmann::json;

void form_cfgs() {
  json program = json::parse(stdin);

  for (auto &f : program["functions"]) {
    // auto blocks = get_named_blocks(f);
    // std::cout<<blocks.dump(2)<<std::endl;
    std::cerr<<"Function: "<<f["name"]<<"\n";
  
    auto blocks = get_named_blocks(f);

    auto cfg = create_cfg(blocks);
  
    print_cfg(cfg, f["name"]);

    std::cerr<<"\n";
  }
}

int main() {
  form_cfgs();
  return 0;
}
