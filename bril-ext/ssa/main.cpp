#include "../cfg/cfg.h"
#include "../form-blocks/form-block.h"
#include "../json.hpp"
#include "../dom/df.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>

// var_def_map object
struct var_n {
  std::string type;
  std::vector<std::string> defs;
};

// rename_map object
struct rename_obj {
  struct st_obj {
  std::string block;
  std::string name;
  };

  size_t i = 0;
  std::stack<st_obj> st;
};

using json = nlohmann::json;
using var_def_map = std::unordered_map<std::string, var_n>;
using name_idx_map = std::unordered_map<std::string, size_t>;
using rename_map = std::unordered_map<std::string, rename_obj>;

name_idx_map get_block_name_idx_map(json &blocks) {
  name_idx_map ni_map;
  for(size_t i = 0; i < blocks.size(); ++i) {
    ni_map[blocks[i]["name"]] = i;
  }
  return ni_map;
}

var_def_map get_var_def_map(json &blocks) {
  // std::cout<<blocks.dump(2);
  var_def_map vd_map;
  for(auto &block : blocks) {
    for(auto &inst: block["insts"]) {
      if(inst.contains("dest")) {
        // std::cout<<inst["dest"]<<", "<<block["name"]<<std::endl;
        vd_map[inst["dest"]].defs.push_back(block["name"]);
        vd_map[inst["dest"]].type = inst["type"];
      }
    }
  }

  // for(auto [k, v] : vd_map) {
  //   std::cout<<k<<std::endl;
  //   std::cout<<"  ";
  //   for(auto b: v) {
  //     std::cout<<b<<" ";
  //   }
  //   std::cout<<std::endl;
  // }

  return vd_map;
}

// block
// cfg-successors
// reverse idoms
void rename(json block, rename_map rmap) {
  // replace each argument with vs[old_name]
  for(auto &inst: block["insts"]) {
    if(inst.contains("args")) {
      for(auto &arg: inst["args"]) {
        arg["name"] = rmap[arg["name"]].st.top().name;
      }
    }

    if(inst.contains("dest")) {
      std::string old_name = inst["dest"];
      inst["dest"] = old_name + std::to_string(rmap[old_name].i++);
      rmap[old_name].st.push({block["name"], inst["dest"]});
    }
  }
}



json insert_phi_nodes(json& function) {
  // get dominance frontier
  // { "block_name" : ["block_df1", ..., "block__dfn"]
  auto df = find_dominance_frontier(function);
  auto blocks = get_named_blocks(function);
  auto vd_map = get_var_def_map(blocks);
  auto ni_map = get_block_name_idx_map(blocks);

  // Insert phi nodes
  std::unordered_map<std::string, std::unordered_set<std::string>> phi_map;
  for (auto &[var, vn]: vd_map) {
    for(size_t i = 0; i < vn.defs.size(); ++i) {
      for(auto &df_name: df[vn.defs[i]]) {
        // Add a phi node to block unless already done
        // Add block to defs[var]
        if(!phi_map[var].contains(df_name)) {
          phi_map[var].insert(df_name);
          auto &block = blocks[ni_map[df_name]];
          json obj;
          obj["dest"] = var;
          obj["op"] = "phi";
          obj["type"] = vn.type;
          block["insts"].insert(block["insts"].begin(), obj);
          vn.defs.emplace_back(df_name);
        }
      }
    }
  }

  // rename phi nodes
  // prepare stack --> add fn args on stack
  
  // std::cout<<blocks.dump(2);

  return blocks;
}

int main() {
  json program = json::parse(stdin);

  for (auto &f : program["functions"]) {
    auto blocks = insert_phi_nodes(f);
  }

  return 0;
}
