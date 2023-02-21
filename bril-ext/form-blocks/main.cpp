#include <iostream>
#include <nlohmann/json.hpp>
#include "form-block.h"

using json = nlohmann::json;

void form_blocks() {
  json program = json::parse(stdin);

  for (auto &f : program["functions"]) {
    auto blocks = get_blocks(f);

    // std::cout<<blocks.dump()<<std::endl;

    for(auto block: blocks) {
      std::cout<<block.dump()<<std::endl;
    }
  }
}

int main() {
  form_blocks();
  return 0;
}
