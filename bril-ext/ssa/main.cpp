#include "../cfg/cfg.h"
#include "../dom/df.h"
#include "../form-blocks/form-block.h"
#include "../json.hpp"

#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

// var_def_map object
struct var_n {
  std::string type;
  std::vector<std::string> defs;
};

// rename_map object
// struct rename_obj {
//   size_t i;
//   std::stack<std::string> st;
// };

using json = nlohmann::json;
using var_def_map = std::unordered_map<std::string, var_n>;
using name_idx_map = std::unordered_map<std::string, size_t>;
using rename_stack = std::unordered_map<std::string, std::stack<std::string>>;
using rename_count = std::unordered_map<std::string, size_t>;

name_idx_map get_block_name_idx_map(json &blocks) {
  name_idx_map ni_map;
  for (size_t i = 0; i < blocks.size(); ++i) {
    ni_map[blocks[i]["name"]] = i;
  }
  return ni_map;
}

var_def_map get_var_def_map(json &blocks) {
  var_def_map vd_map;
  for (auto &block : blocks) {
    for (auto &inst : block["instrs"]) {
      if (inst.contains("dest")) {
        vd_map[inst["dest"]].defs.push_back(block["name"]);
        vd_map[inst["dest"]].type = inst["type"];
      }
    }
  }

  return vd_map;
}

// block
// cfg-successors
// reverse idoms
void rename(rename_stack rmap, std::string bname, json &blocks, rename_count &rcount,
            const cfg_map &cmap, const dom_tree &dtree,
            const name_idx_map &ni_map) {
  // replace each argument with vs[old_name]
  auto bi = ni_map.at(bname);
  for (auto &inst : blocks[bi]["instrs"]) {
    if (inst["op"] != "phi" && inst.contains("args")) {
      for (auto &arg : inst["args"]) {
        if (arg.is_object()) {
          arg["name"] = rmap[arg["name"]].top();
        } else if (arg.is_string()) {
          arg = rmap[arg].top();
        } else {
          throw std::runtime_error("Invalid Json Object - Rename(...)\n");
        }
      }
    }

    if (inst.contains("dest")) {
      std::string old_name = inst["dest"];
      inst["dest"] = old_name + "." + std::to_string(rcount[old_name]++);
      rmap[old_name].push(inst["dest"]);
      // std::cout<<rmap[old_name].top()<<std::endl;
    }
  }

  for (auto succ : cmap.at(bname)) {
    auto si = ni_map.at(succ);
    for (auto &inst : blocks[si]["instrs"]) {
      if (inst["op"] == "phi") {
        // std::cout<<"dest: "<<inst["dest"]<<std::endl;
        std::string dname = inst["dest"];
        dname = dname.substr(0, dname.find("."));
        if (rmap[dname].empty()) {
          inst["args"].push_back("__undefined");
        } else {
          inst["args"].push_back(rmap[dname].top());
        }
        inst["labels"].push_back(bname);
      }
    }
  }

    for (auto sub : (dtree.contains(bname) ? dtree.at(bname) : std::initializer_list<std::string>() )) {
      rename(rmap, sub, blocks, rcount, cmap, dtree, ni_map);
    }
}

void insert_phi_nodes(json &function) {
  // get dominance frontier
  // { "block_name" : ["block_df1", ..., "block__dfn"]
  auto df = find_dominance_frontier(function);
  auto blocks = get_named_blocks(function);
  auto vd_map = get_var_def_map(blocks);
  auto ni_map = get_block_name_idx_map(blocks);

  // Insert phi nodes
  std::unordered_map<std::string, std::unordered_set<std::string>> phi_map;
  for (auto &[var, vn] : vd_map) {
    for (size_t i = 0; i < vn.defs.size(); ++i) {
      for (auto &df_name : df[vn.defs[i]]) {
        // Add a phi node to block unless already done
        // Add block to defs[var]
        if (!phi_map[var].contains(df_name)) {
          phi_map[var].insert(df_name);
          auto &block = blocks[ni_map[df_name]];
          json obj;
          obj["dest"] = var;
          obj["op"] = "phi";
          obj["type"] = vn.type;
          obj["args"] = json::array();
          obj["labels"] = json::array();
          block["instrs"].insert(block["instrs"].begin(), obj);
          vn.defs.emplace_back(df_name);
        }
      }
    }
  }

  // rename phi nodes
  // prepare stack --> add fn args on stack
  rename_stack rst;
  rename_count rcount;
  auto cmap = create_cfg(blocks);
  auto dtree = create_dominator_tree(function);
  for (auto arg : function["args"]) {
    rst[arg["name"]].push(arg["name"]);
  }
  rename(rst, blocks[0]["name"], blocks, rcount, cmap, dtree, ni_map);

  // std::cout<<blocks.dump(2);

  json instrs = json::array();
  for(auto &block: blocks) {
    json label;
    label["label"] = block["name"];
    instrs.push_back(label);
    for(auto &&inst: block["instrs"]) {
      instrs.push_back(inst); 
    }
  }

  function["instrs"] = std::move(instrs);
}

int main() {
  json program = json::parse(stdin);

  for (auto &f : program["functions"]) {
    insert_phi_nodes(f);
  }


  std::cout<<program.dump(2)<<std::endl;

  return 0;
}
