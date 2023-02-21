#include "form-block.h"

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
