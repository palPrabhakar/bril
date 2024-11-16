#include "../cfg/cfg.h"
#include "../form-blocks/form-block.h"
#include "../json.hpp"
#include "df.h"

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
  for (auto [k, v] : predm) {
    if (v.empty())
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

  for (auto blk : blocks) {
    nblocks.push_back(blk);
  }

  blocks = nblocks;
}

// A function to find set of dominators for each block
// A block b is dominated by block d if block d lies along all the path from
// entry to block d Note: Under this definition of dominance every node
// dominates itself Input: Procedure f Output: For each block b in f, the set of
// blocks that dominate b
dom_map find_dominators(json &f) {
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

  // Check the predecessor graph
  // If none of the blocks have empty predecessor graph then insert a unique
  // entry block
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

  // for (auto block : blocks) {
  //   std::cerr << block["name"] << ":\n";
  //   for (auto blk : doms[block["name"]]) {
  //     std::cerr << "  " << blk << "\n";
  //   }
  //   std::cerr << "\n";
  // }

  return doms;
}

std::string get_idom(dom_map dom, std::string bname) {
  // get blocks dominators
  std::string idom;
  auto bdoms = dom[bname];
  // create a copy of block dominators
  auto idom_set = bdoms;

  // walk through the dominators of the blocks
  // find their dominators and remove them from the idom_set
  // the last remaining element must be the idom of the block
  for (auto bd : bdoms) {
    for (auto d : dom[bd]) {
      idom_set.erase(d);
    }
  }

  if (!idom_set.empty()) {
    idom = *idom_set.begin();
  }

  return idom;
}


idom_map get_idom_map(json &f) {
  // the list of blocks the algorithm is looking at
  auto dom = find_dominators(f);

  // create strict dominator set
  for (auto &[k, v] : dom) {
    v.erase(k);
  }

  idom_map idom;

  for (auto [k, v] : dom) {
    idom[k] = get_idom(dom, k);
  }

  // std::cerr<<"idom set\n";

  // for (auto [k, v] : idom) {
  //   std::cerr << "block: " << k << ", idom: " << v << "\n";
  // }

  return idom;
}


// A function to create the dominator tree
// Dominator tree captures the dominance information in  a tree
// Every block has a node in the tree and the edges goes from the immediate
// dominator to the dominee. Node n is the immediate dominator of node m if
// there is no other node in between m and n.
// Input: function f
dom_tree create_dominator_tree(json &f) {
  // the list of blocks the algorithm is looking at
  auto dom = find_dominators(f);

  // create strict dominator set
  for (auto &[k, v] : dom) {
    v.erase(k);
  }

  dom_tree dtree;

  for (auto [k, v] : dom) {
    auto idom = get_idom(dom, k);
    dtree[idom].push_back(k);
  }

  // std::cerr<<"dominator tree algorithm halted\n";

  return dtree;
}

// Dominance frontier
// The dominance frontier of a node n is the set of nodes that are just outside
// the dom_set of that nodes. The set of nodes that lie on a path from the node
// n to exit but are not in the dom_set of the node (only the first node, not
// it's successors).
df_map find_dominance_frontier(json &f) {
  auto blocks = get_named_blocks(f);

  // control flow graph
  auto cfgm = create_cfg(blocks);

  auto predm = get_predecessor_map(cfgm);

  insert_entry_block_if_required(predm, blocks);
  
  auto idoms = get_idom_map(f);

  df_map df;

  for(auto [k, v]: predm) {
    if(v.size() > 1) {
      for(auto pred: v) {
        auto runner = pred;
        while(runner != idoms[k]) {
          df[runner].push_back(k);
          runner = idoms[runner];
        }
      }
    }
  }

  // std::cerr<<"dominance frontier algorithm halted\n";
  //
  // for(auto [k, v]: df) {
  //   std::cerr<<k<<": \n";
  //   for(auto n: v) {
  //     std::cerr<<"  "<<n;
  //   }
  //   std::cerr<<"\n";
  // }

  return df;

}


