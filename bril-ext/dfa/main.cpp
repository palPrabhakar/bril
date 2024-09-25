#include "../cfg/cfg.h"
#include "../form-blocks/form-block.h"
#include "../json.hpp"
#include <iostream>
#include <pthread.h>
#include <unordered_map>
#include <unordered_set>

using string_set = std::unordered_set<std::string>;
using string_map = std::unordered_map<std::string, string_set>;

// maps the name of the block with the idx in the json array
std::unordered_map<std::string, int> create_block_lookup_map(json &blocks) {
  std::unordered_map<std::string, int> lookup;

  for (auto i = 0; i < blocks.size(); ++i) {
    lookup[blocks[i]["name"]] = i;
  }

  return lookup;
}

// creates the entry block for the procedure
// sets it to any arguments passed to the procedure
void create_entry_block(json &blocks, json &f) {
  json eblock;
  eblock["name"] = "entry";
  if (f.contains("args")) {
    eblock["args"] = f["args"];
  }

  blocks.push_back(eblock);
}

// calculates the kill_set
// the set of available definitions that got redefined in the current set
string_set get_kill_set(json block, string_set in_set) {
  string_set kill_set;

  for (auto inst : block["insts"]) {
    if (inst.contains("dest")) {
      auto dest = inst["dest"];
      if (in_set.find(dest) != in_set.end()) {
        kill_set.insert(dest);
      }
    }
  }

  return kill_set;
}

// calculates the gen_set of a block
// the set of new def in the block
string_set get_gen_set(json block) {
  string_set gen_set;
  
  // std::cerr<<"\n\n"<<block.dump(2)<<"\n\n";

  for (auto inst : block["insts"]) {
    if (inst.contains("dest")) {
      auto dest = inst["dest"];
      gen_set.insert(dest);
    }
  }
  return gen_set;
}

string_set do_set_difference(string_set s1, string_set s2) {
  string_set result_set;
  for (auto def : s1) {
    if (s2.find(def) == s2.end()) {
      result_set.insert(def);
    }
  }
  return result_set;
}

string_set do_set_union(string_set s1, string_set s2) {
  string_set result_set = s1;
  for (auto def : s2) {
    result_set.insert(def);
  }

  return result_set;
}

// Calculates the reaching definitions
// Reaching definitions: At a program point p, the set of all definitions that are
// available at that point is the set of reaching definitions at that point.
// Input:
// json function
// Output:
//
void find_reaching_definitions(json &f) {
  // the list of blocks the algorithm is looking at
  string_set worklist;

  // the set of variables defined at the entry of the block
  // union of out_set of all the predecessors
  string_map in;

  // the set of variables defined at the exit of the block
  // out_set = (in_set - kill_set) Un gen_set
  // gen_set: the set of definitions that are defined the in the block
  // kill_set: the set of definitions previously defined that got redefined in
  // the block
  string_map out;

  auto blocks = get_named_blocks(f);

  // control flow graph
  auto cfgm = create_cfg(blocks);
  // std::cerr << "Control Flow Graph\n";
  // print_cfg(cfgm, f["name"]);

  // predecessor map
  auto predm = get_predecessor_map(cfgm);
  // std::cerr << "Predecessor Map\n";
  // print_cfg(predm, f["name"]);

  // create the worklist
  for (auto block : blocks) {
    worklist.insert(block["name"]);
  }

  create_entry_block(blocks, f);
  // update the predecessor map for the first block and set it to entry block
  predm[blocks[0]["name"]].push_back("entry");

  auto blookup = create_block_lookup_map(blocks);

  // init entry set
  auto idx = blookup["entry"];
  if (blocks[idx].contains("args")) {
    for (auto arg : blocks[idx]["args"]) {
      in["entry"].insert(arg["name"]);
      out["entry"].insert(arg["name"]);
    }
  } else {
    in["entry"] = std::unordered_set<std::string>();
    out["entry"] = std::unordered_set<std::string>();
  }

  while (!worklist.empty()) {
    auto bname = *worklist.begin();
    // std::cerr << "Processing block: " << bname << std::endl;
    auto idx = blookup[bname];
    worklist.erase(bname);

    // check for the predecessor
    // Iterates over the predecessor vector
    for (auto pred : predm[bname]) {
      // merge
      // iterate over all the definitions in the out_set of the predecessors
      // and add them to the in_set of the current block;
      for (auto def : out[pred]) {
        in[bname].insert(def);
      }
    }
    // transfer
    auto kill_set = get_kill_set(blocks[idx], in[bname]);
    auto gen_set = get_gen_set(blocks[idx]);
    auto new_out =
        do_set_union(gen_set, do_set_difference(in[bname], kill_set));
    if (new_out != out[bname]) {
      out[bname] = new_out;

      for (auto succ : cfgm[bname]) {
        worklist.insert(succ);
      }
    }
  }

  // std::cerr << "worklist algorithm halted\n";
  
  for(auto [k, v] : blookup) {
    std::cerr<<k<<":\n";
    std::cerr<<"  in: ";
    for(auto def: in[k]) {
      std::cerr<<def<<" ";
    }
    std::cerr<<"\n";

    std::cerr<<"  out: ";
    for(auto def: out[k]) {
      std::cerr<<def<<" ";
    }
    std::cerr<<"\n\n";
  }
}

// Do data flow analysis
void do_dfa() {
  json program = json::parse(stdin);

  // std::ifstream file("cond-args.json");
  // json program = json::parse(file);

  for (auto &f : program["functions"]) {
    find_reaching_definitions(f);
  }
}

int main() {
  // Reaching definitions
  do_dfa();

  return 0;
}
