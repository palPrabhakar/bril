#include "../cfg/cfg.h"
#include "../form-blocks/form-block.h"
#include "../json.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using dom_map =
    std::unordered_map<std::string, std::unordered_set<std::string>>;

std::unordered_set<std::string>
get_incoming_dominators(std::string bname, cfg_map predm, dom_map &dmap) {

  if (predm[bname].empty())
    return {};

  // add everything in the first predecssor to the incoming_dom_set
  std::unordered_set<std::string> doms = dmap[predm[bname][0]];

  for (auto i = 1; i < predm[bname].size(); ++i) {
    auto pred = predm[bname][i];
    for (auto it = doms.begin(); it != doms.end();) {
      if (dmap[pred].find(*it) == dmap[pred].end()) {
        it = doms.erase(it);
      } else {
        ++it;
      }
    }
  }

  return doms;
}

std::unordered_set<std::string> get_maximal_dom_set(json &blocks) {
  std::unordered_set<std::string> max_set;
  for (auto block : blocks) {
    max_set.insert(block["name"]);
  }
  return max_set;
}

void init_doms(dom_map &map, json &blocks) {
  auto max_set = get_maximal_dom_set(blocks);

  for (auto blk : max_set) {
    map[blk] = max_set;
  }

  map[blocks[0]["name"]] = {blocks[0]["name"]};
}

void insert_entry_block_if_required(cfg_map &predm, json &blocks) {
  bool required = true;

  for(auto [k, v]: predm) {
    if(v.empty())
      return;
  }

  json eblock;
  eblock["name"] = "entry";

  predm["entry"] = {};


  // current entry block
  auto ceb = blocks[0]["name"];
  predm[ceb].push_back("entry");
  
  // Not best thing to do
  json::array_t nblocks;
  nblocks.push_back(eblock);
    
  for(auto blk: blocks) {
    nblocks.push_back(blk);
  }
  
  blocks = nblocks;
}

// A function to find set of dominators for each block
// A block b is dominated by block d if block d lies along all the path from
// entry to block d Note: Under this definition of dominance every node
// dominates itself Input: Procedure f Output: For each block b in f, the set of
// blocks that dominate b
void find_dominators(json &f) {
  // the list of blocks the algorithm is looking at
  auto blocks = get_named_blocks(f);
  // std::cerr<<blocks.dump(2)<<"\n";

  // control flow graph
  auto cfgm = create_cfg(blocks);
  // std::cerr << "control flow graph\n";
  // print_cfg(cfgm, f["name"]);

  auto predm = get_predecessor_map(cfgm);
  // std::cerr << "predecessor map\n";
  // print_cfg(predm, f["name"]);
  
  // TODO:
  // Check the predecessor graph
  // If none of the blocks have empty predecessor graph then insert a unique entry block
  insert_entry_block_if_required(predm, blocks);

  dom_map doms;
  init_doms(doms, blocks);
  // doms[blocks[0]["name"]] = {blocks[0]["name"]};
  // std::cerr<<"entry block: "<<blocks[0]["name"]<<"\n";

  bool changed = true;
  while (changed) {
    changed = false;

    for (auto block : blocks) {
      std::string bname = block["name"];

      auto dom = get_incoming_dominators(bname, predm, doms);
    
      dom.insert(bname);

      if (dom != doms[bname]) {
        doms[bname] = dom;
        changed = true;
      }
    }
  }

  // std::cerr << "dominator algorithm halted\n\n";

  for (auto block : blocks) {
    std::cerr << block["name"] << ":\n";
    for (auto blk : doms[block["name"]]) {
      std::cerr << "  " << blk << "\n";
    }
    std::cerr << "\n";
  }
}

// Do dominator analysis
void do_dom_analysis() {
  json program = json::parse(stdin);

  // std::ifstream file("loopcond.json");
  // json program = json::parse(file);

  for (auto &f : program["functions"]) {
    find_dominators(f);
  }
}

int main() {
  // Reaching definitions
  do_dom_analysis();

  return 0;
}
