#include <iostream>
#include <nlohmann/json.hpp>

const std::string JMP = "jmp";
const std::string BR = "br";

using json = nlohmann::json;

json get_blocks(json &f) {
  json::array_t blocks;
  json::array_t block;

  for (auto inst : f["instrs"]) {
    if (inst.contains("label")) {
      if (!block.empty()) {
        blocks.insert(blocks.end(), block);
        block.clear();
      }
    } else {
      if (inst["op"] == BR || inst["op"] == JMP) {
        block.insert(block.end(), inst);
        blocks.insert(blocks.end(), block);
        block.clear();
      } else {
        block.insert(block.end(), inst);
      }
    }
  }

  if(!block.empty()) {
    blocks.insert(blocks.end(), block);
  } 

  return blocks;
}

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
