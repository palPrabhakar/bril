#include "../json.hpp"
#include "../cfg/cfg.h"
#include "../form-blocks/form-block.h"
#include <iostream>
#include <pthread.h>
#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string, int> create_block_lookup_map(json &blocks) {
  std::unordered_map<std::string, int> lookup;

  for(auto i = 0; i < blocks.size(); ++i) {
    lookup[blocks[i]["name"]] = i;
  }

  return lookup;
}

void create_entry_block(json &blocks, json &f) {
  json eblock;
  eblock["name"] = "entry";
  if(f.contains("args")) {
    eblock["args"] = f["args"];
  }

  blocks.push_back(eblock); 
}

// Parameters:
// named blocks // predecessor map
void find_reaching_definitions(json &f) {
  std::unordered_set<std::string> worklist;
  std::unordered_map<std::string, std::unordered_set<std::string>> in;
  std::unordered_map<std::string, std::unordered_set<std::string>> out;

  auto blocks = get_named_blocks(f); 

  // control flow graph
  auto cfgm = create_cfg(blocks);

  // predecessor map
  auto predm = get_predecessor_map(cfgm);
  // print_cfg(pred_map, f["name"]);
  
  // create the worklist
  for(auto block: blocks) {
    worklist.insert(block["name"]);
  }

  create_entry_block(blocks, f);

  auto blookup = create_block_lookup_map(blocks);
 
  // init entry set
  auto idx = blookup["entry"];
  if(blocks[idx].contains("args")) {
    for(auto arg: blocks[idx]["args"]) {
      in["entry"].insert(arg["name"]);
      out["entry"].insert(arg["name"]);
    }
  } else {
    in["entry"] = std::unordered_set<std::string>();
    out["entry"] = std::unordered_set<std::string>();
  }

  while(!worklist.empty()) {
    for(auto bname: worklist) {
      worklist.erase(bname);
      
      // check for the predecessor
      // for(auto pred: predm[bname]) {
      //   auto pidx = blookup[pred];

      // }
    }
  }

}

// Do data flow analysis
void do_dfa() {
  json program = json::parse(stdin);

  for(auto &f: program["functions"]) {
    find_reaching_definitions(f);
  }
}

int main() {
  // Reaching definitions
  do_dfa();

  return 0;
}
