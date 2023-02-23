#include "../form-blocks/form-block.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

// Current known issues 
// The pratice of assigning a unique name to each canonical value doesn't work
// The problem is when there are more than one basic block in the function 
// In case of reassignment, the value might be used outside the basic block 
// In such case the reference for the variable is lost 

// The assertion in the else case is wrong 
// Again for the reason specified above
// Works only for a function with single basic block 

// Doesn't work with jmp cases 
// Deosn't work with relationals 
// Doesnt' work with any types other than int


struct Node {
  std::string op;
  std::string var;
  int op1 = 0, op2 = 0;
};

using json = nlohmann::json;
// using tb_it = std::map<Node, std::string>::const_iterator;

void lvn(json &block) {
  if(block[0].contains("label"))
    return;
  
  int count = 0;
  // set to hold all the variables that are assigned
  // For every instruction create a node
  std::vector<Node> lvn_tb; // inst --> node
  std::unordered_map<std::string, int> node_lookup; // stores the unique op code for every unique instr
  std::unordered_map<std::string, int> variables; // for every assigned variable the current 
  // int idx = 0;

  std::string op;

  for (auto &inst : block) {
    // Add a entry in the lvn_tb for each inst
    // std::cerr<<inst.dump(2)<<std::endl;
    Node lvn_node;
    op = inst["op"];
    lvn_node.op = op;

    if (op == "const") {
      // std::cerr<<"CONST"<<std::endl;
      lvn_node.op1 = inst["value"];
      op += std::to_string(lvn_node.op1);
      op += std::to_string(lvn_node.op2);
    } else if (op == "jmp") {
      // op += inst["labels"][0];
      op += std::to_string(lvn_node.op1);
      op += std::to_string(lvn_node.op2);
    } else {
      // std::cerr<<"NON CONST"<<std::endl;
      assert(variables.find(inst["args"][0]) != variables.end());
      lvn_node.op1 = variables[inst["args"][0]];
      op += std::to_string(lvn_node.op1);
      if (inst["args"].size() == 2) {
        assert(variables.find(inst["args"][1]) != variables.end());
        lvn_node.op2 = variables[inst["args"][1]];
      }
      op += std::to_string(lvn_node.op2);
    }

    // std::cerr<<"OP CODE: "<<op<<std::endl;

    if (node_lookup.find(op) == node_lookup.end()) {
      if (inst.contains("dest")) {
        // check if the variable already exist 
        auto key = inst["dest"];
        if(variables.find(key) != variables.end()) {
          // Already exist    
          auto idx = variables[key];
          lvn_tb[idx].var = "lvn." + std::to_string(count++);
        } 
        lvn_node.var = key;
        lvn_tb.push_back(lvn_node);
        variables[key] = lvn_tb.size() - 1;
      } else
        lvn_tb.push_back(lvn_node);

      node_lookup.insert({op, lvn_tb.size() - 1});
    } else {
      if (inst.contains("dest")) {
        lvn_node.var = lvn_tb[node_lookup[op]].var;
        lvn_tb.push_back(lvn_node);
        variables[inst["dest"]] = node_lookup[op];
      } else
        lvn_tb.push_back(lvn_node);
    }
  }

  for (int i = 0; i < lvn_tb.size(); ++i) {
    if(block[i]["op"] == "jmp")
      continue;

    op = lvn_tb[i].op;
    op += std::to_string(lvn_tb[i].op1);
    op += std::to_string(lvn_tb[i].op2);
    // std::cerr << "OP CODE: " << op << std::endl;
    if (node_lookup[op] != i) {
      // Node shown up before
      block[i]["op"] = "id";
      block[i]["args"].clear();
      int idx = node_lookup[op];
      block[i]["args"].push_back(lvn_tb[idx].var);
    } else {
      // if op const do nothing
      if (block[i]["op"] != "const") {
        // block[i]["args"][0] = lvn[lvn_tb[i].op1]
        if(block[i].contains("dest")) {
          block[i]["dest"] = lvn_tb[i].var;
        }
        block[i]["args"][0] = lvn_tb[lvn_tb[i].op1].var;

        if (block[i]["args"].size() == 2) {
          block[i]["args"][1] = lvn_tb[lvn_tb[i].op2].var;
        }
      } else {
        block[i]["dest"] = lvn_tb[i].var;
      }
    }
  }
}

void optimize_function(json &f) {
  json blocks = get_blocks(f);

  for (auto &block : blocks) {
    lvn(block);
  }

  // std::cerr << blocks.dump(2) << std::endl;
  f["instrs"].clear();
  for(auto block: blocks) {
    for(auto inst: block) {
      f["instrs"].push_back(inst);
    }
  }

}

void do_lvn() {
  json program = json::parse(stdin);

  for (auto &f : program["functions"]) {
    optimize_function(f);
  }

  std::cout << program.dump(2);
}

int main() {

  do_lvn();

  return 0;
}
