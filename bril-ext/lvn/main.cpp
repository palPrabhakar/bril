#include "../form-blocks/form-block.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

// Local Value Numbering Implementation
// The current implementation has:
// 1. Trivial common subexpression elimination
// 2. Trivial copy propogation

struct Node {
  bool op1_first;
  bool op2_first;
  std::string op;
  std::string var;
  std::string op1;
  std::string op2;
};

using json = nlohmann::json;
// using tb_it = std::map<Node, std::string>::const_iterator;
//

void analyze_block(json &block, std::vector<Node> &lvn_tb,
                   std::unordered_map<std::string, int> &node_lookup,
                   std::unordered_map<std::string, int> &variables,
                   int &count) {
  std::string op;

  for (auto &inst : block) {
    // Add a entry in the lvn_tb for each inst
    // std::cerr<<inst.dump(2)<<std::endl;
    Node lvn_node;
    op = inst["op"];
    lvn_node.op = op;

    if (op == "const") {
      if (inst["type"] == "int") {
        lvn_node.op1 = std::to_string(static_cast<int>(inst["value"]));
      } else {
        lvn_node.op1 = inst["value"] ? "true" : "false";
      }
      lvn_node.op1_first = true;
      op += lvn_node.op1;
      op += lvn_node.op2;
    } else if (op == "jmp") {
      op += lvn_node.op1;
      op += lvn_node.op2;
    } else {
      if (variables.find(inst["args"][0]) != variables.end()) {
        lvn_node.op1 = std::to_string(variables[inst["args"][0]]);
        lvn_node.op1_first = false;
      } else {
        lvn_node.op1 = inst["args"][0];
        lvn_node.op1_first = true;
      }
      op += lvn_node.op1;

      if (inst["args"].size() == 2) {
        if (variables.find(inst["args"][1]) != variables.end()) {
          lvn_node.op2 = std::to_string(variables[inst["args"][1]]);
          lvn_node.op2_first = false;
        } else {
          lvn_node.op2 = inst["args"][1];
          lvn_node.op2_first = true;
        }
      }
      op += lvn_node.op2;
    }

    // std::cerr << "OP CODE: " << op << std::endl;

    if (node_lookup.find(op) == node_lookup.end()) {
      if (inst.contains("dest")) {
        // check if the variable already exist
        auto key = inst["dest"];
        if (variables.find(key) != variables.end()) {
          // Already exist
          auto idx = variables[key];
          lvn_tb[idx].var = "lvn." + std::to_string(count++);
        }
        lvn_node.var = key;
        lvn_tb.push_back(lvn_node);
        variables[key] = lvn_tb.size() - 1;
      } else {
        lvn_tb.push_back(lvn_node);
      }
      node_lookup.insert({op, lvn_tb.size() - 1});
    } else {
      if (inst.contains("dest")) {
        lvn_node.var = lvn_tb[node_lookup[op]].var;
        lvn_tb.push_back(lvn_node);
        variables[inst["dest"]] = node_lookup[op];
      } else {
        lvn_tb.push_back(lvn_node);
      }
    }
  }
}

void get_arg1(json &block, std::vector<Node> &lvn_tb, int i) {
  std::string dest;
  Node cur = lvn_tb[i];
  while(!cur.op1_first) {
    // std::cerr<<cur.var<<std::endl;
    cur = lvn_tb[std::stoi(cur.op1)];
    if(cur.op != "id")
      break;
  }
  block[i]["args"][0] = cur.var;
}

void get_arg2(json &block, std::vector<Node> &lvn_tb, int i) {
  std::string dest;
  Node cur = lvn_tb[i];
  while (!cur.op2_first) {
    // std::cerr<<cur.var<<std::endl;
    cur = lvn_tb[std::stoi(cur.op2)];
    if(cur.op != "id")
      break;
  }
  block[i]["args"][1] = cur.var;
}

void modify_block(json &block, std::vector<Node> &lvn_tb,
                  std::unordered_map<std::string, int> &node_lookup,
                  std::unordered_map<std::string, int> &variables) {
  std::string op;
  for (int i = 0; i < lvn_tb.size(); ++i) {
    if (block[i]["op"] == "jmp")
      continue;

    op = lvn_tb[i].op;
    op += lvn_tb[i].op1;
    op += lvn_tb[i].op2;
    // std::cerr << "OP CODE: " << op << std::endl;
    if (node_lookup[op] != i) {
      // Node shown up before
      block[i]["op"] = "id";
      block[i]["args"].clear();
      int idx = node_lookup[op];
      block[i]["args"].push_back(lvn_tb[idx].var);
    } else {
      // if op const do nothing
      if (block[i]["op"] == "id") {
        block[i]["dest"] = lvn_tb[i].var;
        get_arg1(block, lvn_tb, i); 
      } else if (block[i]["op"] != "const") {
        // block[i]["args"][0] = lvn[lvn_tb[i].op1]
        if (block[i].contains("dest")) {
          block[i]["dest"] = lvn_tb[i].var;
        }

        if (lvn_tb[i].op1_first) {
          block[i]["args"][0] = lvn_tb[i].op1;
        } else {
          get_arg1(block, lvn_tb, i);
          // block[i]["args"][0] = lvn_tb[std::stoi(lvn_tb[i].op1)].var;
        }
        if (block[i]["args"].size() == 2) {
          if (lvn_tb[i].op2_first) {
            block[i]["args"][1] = lvn_tb[i].op2;
          } else {
            // block[i]["args"][1] = lvn_tb[std::stoi(lvn_tb[i].op2)].var;
            get_arg2(block, lvn_tb, i);
          }
        }
      } else {
        block[i]["dest"] = lvn_tb[i].var;
      }
    }
  }
}

void lvn(json &block) {
  // std::cerr<<"Processing block\n";

  if (block[0].contains("label"))
    return;

  int count = 0;
  // set to hold all the variables that are assigned
  // For every instruction create a node
  std::vector<Node> lvn_tb; // inst --> node
  std::unordered_map<std::string, int>
      node_lookup; // stores the unique op code for every unique instr
  std::unordered_map<std::string, int>
      variables; // for every assigned variable the current

  analyze_block(block, lvn_tb, node_lookup, variables, count);

  modify_block(block, lvn_tb, node_lookup, variables);
}

void optimize_function(json &f) {
  json blocks = get_blocks(f);

  for (auto &block : blocks) {
    lvn(block);
  }

  // std::cerr << blocks.dump(2) << std::endl;
  f["instrs"].clear();
  for (auto block : blocks) {
    for (auto inst : block) {
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
