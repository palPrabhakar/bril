#include "form-block.h"
#include <cstdio>
#include <string>

json get_named_blocks(json &f) {
  json::array_t blocks =  get_blocks(f);
  json ret_blocks;

  int bi = 0;
  std::string name = "";
  for(int i = 0; i < blocks.size(); ++i) {
    if(blocks[i].size() == 1 && blocks[i][0].contains("label")) {
      name = blocks[i][0]["label"];
      ret_blocks[name] = blocks[++i];
    } else {
      name = "block" + std::to_string(bi++);
      ret_blocks[name] = blocks[i];
    }
  }
    
  return ret_blocks;
}

json get_blocks(json &f) {
  json::array_t blocks;
  json::array_t block;

  for (auto inst : f["instrs"]) {
    if (inst.contains("label")) {
      if (!block.empty()) {
        blocks.insert(blocks.end(), block);
        block.clear();
      }
      block.insert(block.end(), inst);
      blocks.insert(blocks.end(), block);
      block.clear();
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
