#include <iostream>
#include "../json.hpp"
#include "../form-blocks/form-block.h"

using json = nlohmann::json;

void form_blocks() {
  json program = json::parse(stdin);

  for (auto &f : program["functions"]) {
    // auto blocks = get_blocks(f);
    // for(auto block: blocks) {
    //   std::cout<<"Block\n";
    //   std::cout<<block.dump(2)<<std::endl;
    // }

    auto blocks = get_named_blocks(f);
    std::cout<<blocks.dump(2)<<std::endl;

  }
}

int main() {
  form_blocks();
  return 0;
}
