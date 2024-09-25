#include "../form-blocks/form-block.h"
#include "../json.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
// #include <algorithm>

// Local Value Numbering Implementation
// The current implementation has:
// 1. Trivial common subexpression elimination
// 2. Trivial copy propagation
// 3. CSE commutivity
// 4. Trivial constant folding

// Quite a poor algorithm design..LOL

struct Node {
  // If the operand1 if defined for the first time in the block
  bool op1_first;
  // If the operand2 if defined for the first time in the block
  bool op2_first;
  std::string op; // operator
  // canonical variable for the operation.
  std::string var;
  std::string op1; // operand 1
  std::string op2; // operand 2
};

using json = nlohmann::json;
using _lvn_tb = std::vector<Node>;
using _node_map = std::unordered_map<std::string, int>;
using _variable_map = std::unordered_map<std::string, int>;
//

void print_lvn_node(Node lvn_node) {
  std::cerr << "Printing LVN_NODE:" << std::endl;
  std::cerr << "lvn_node.op: " << lvn_node.op << std::endl;
  std::cerr << "lvn_node.op1: " << lvn_node.op1 << std::endl;
  std::cerr << "lvn_node.op2: " << lvn_node.op2 << std::endl;
  std::cerr << "lvn_node.var: " << lvn_node.var << std::endl;
  std::cerr << std::endl;
}
void analyze_block(json &block, _lvn_tb &lvn_tb, _node_map &node_lookup,
                   _variable_map &variables, int &count) {
  std::string op;  // unique op code for identify operations
  std::string op2; // for cse commutivity "add", "mul"

  for (auto &inst : block) {
    // Add a entry in the lvn_tb for each inst
    // std::cerr<<inst.dump(2)<<std::endl;
    Node lvn_node;
    op = inst["op"];
    op2 = "";
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
      if (lvn_node.op == "add" || lvn_node.op == "mul") {
        op2 = lvn_node.op + lvn_node.op2 + lvn_node.op1;
      }
    }

    // std::cerr << "OP CODE: " << op << std::endl;
    // std::cerr << "OP CODE: " << op2 << std::endl;
    if (node_lookup.find(op) != node_lookup.end()) {
      if (inst.contains("dest")) {
        lvn_node.var = lvn_tb[node_lookup[op]].var;
        lvn_tb.push_back(lvn_node);
        variables[inst["dest"]] = node_lookup[op];
      } else {
        lvn_tb.push_back(lvn_node);
      }
    } else if (op2 != "" && node_lookup.find(op2) != node_lookup.end()) {
      if (inst.contains("dest")) {
        lvn_node.var = lvn_tb[node_lookup[op2]].var;
        std::swap(lvn_node.op1, lvn_node.op2);
        lvn_tb.push_back(lvn_node);
        variables[inst["dest"]] = node_lookup[op2];
      } else {
        lvn_tb.push_back(lvn_node);
      }
    } else {
      if (inst.contains("dest")) {
        // check if the variable already exist
        auto key = inst["dest"];
        if (variables.find(key) != variables.end()) {
          // Already exist
          auto idx = variables[key];
          lvn_tb[idx].var = "lvn." + std::to_string(count++);
          variables[lvn_tb[idx].var] = idx;
        }
        lvn_node.var = key;
        lvn_tb.push_back(lvn_node);
        variables[key] = lvn_tb.size() - 1;
      } else {
        lvn_tb.push_back(lvn_node);
      }
      node_lookup.insert({op, lvn_tb.size() - 1});
    }
  }
}

// For trivial copy propogation
void get_arg1(json &block, _lvn_tb &lvn_tb, int i) {
  std::string dest;
  Node cur = lvn_tb[i];

  if (cur.op1_first) {
    block[i]["args"][0] = cur.op1;
    return;
  }

  while (!cur.op1_first) {
    // std::cerr<<cur.var<<std::endl;
    cur = lvn_tb[std::stoi(cur.op1)];
    if (cur.op != "id")
      break;
  }
  block[i]["args"][0] = cur.var;
}

// For trivial copy propogation
void get_arg2(json &block, _lvn_tb &lvn_tb, int i) {
  std::string dest;
  Node cur = lvn_tb[i];

  if (cur.op2_first) {
    block[i]["args"][1] = cur.op2;
    return;
  }

  while (!cur.op2_first) {
    // std::cerr<<cur.var<<std::endl;
    cur = lvn_tb[std::stoi(cur.op2)];
    if (cur.op != "id")
      break;
  }
  block[i]["args"][1] = cur.var;
}

void do_constant_propogation(json &block, _lvn_tb &lvn_tb,
                             _variable_map &variables, int i) {
  std::string arg1 = static_cast<std::string>(block[i]["args"][0]);
  if (variables.find(arg1) == variables.end()) {
    return;
  }
  int idx_arg1 = variables[arg1];

  // std::cerr<<"arg1: "<<arg1<<", "<<lvn_tb[idx_arg1].op<<std::endl;
  // std::cerr<<"\n"<<block[i].dump(2)<<"\n\n";

  if (lvn_tb[idx_arg1].op == "const") {
    block[i]["op"] = "const";
    block[i]["args"].clear();

    if (lvn_tb[idx_arg1].op1 == "true" || lvn_tb[idx_arg1].op1 == "false") {
      bool value = lvn_tb[idx_arg1].op1 == "true" ? true : false;
      block[i]["value"] = value;
    } else {
      // std::cerr<<"constant prop: "<<lvn_tb[idx_arg1].op1<<"\n\n";
      block[i]["value"] = std::stoi(lvn_tb[idx_arg1].op1);
    }
  }
}

bool is_foldable_operation(std::string op) {
  if (op == "add" || op == "mul" || op == "and" || op == "or" || op == "not" || op == "eq" || op == "le" || op == "lt" || op == "gt" || op == "ge") {
    return true;
  }

  return false;
}

bool is_argument_constant(std::string arg, _lvn_tb &lvn_tb,
                          _variable_map &variables, int &ret_val) {
  if (variables.find(arg) != variables.end()) {
    int idx = variables[arg];
    if (lvn_tb[idx].op == "const") {
      if (lvn_tb[idx].op1 == "true") {
        ret_val = 1;
      } else if (lvn_tb[idx].op1 == "false") {
        ret_val = 0;
      } else {
        ret_val = std::stoi(lvn_tb[idx].op1);
      }
      return true;
    }
  }
  return false;
}

void do_constant_folding(json &block, _lvn_tb &lvn_tb, _variable_map &variables,
                         int i) {

  std::string op = block[i]["op"];
  if (!is_foldable_operation(op)) {
    return;
  }

  // Do constant folding for not
  if(op == "not")
    return;

  std::string arg1 = block[i]["args"][0];
  std::string arg2 = block[i]["args"][1];
  int val1, val2;
  if (is_argument_constant(arg1, lvn_tb, variables, val1) &&
      is_argument_constant(arg2, lvn_tb, variables, val2)) {
    // Do folding
    block[i]["args"].clear();
    if(op == "add") {
      int add = val1+val2;
      block[i]["value"] = add;
      lvn_tb[i].op1 = std::to_string(add);
    }
    else if (op == "mul") {
      int prod = val1*val2;
      block[i]["value"] = std::to_string(prod);
      lvn_tb[i].op1 = std::to_string(prod); 
    } else if (op == "and") {
      if(val1 == 0) {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
      else if (val2 == 1) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      } else {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
    } else if (op == "or") {
      if(val1 == 1) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      }
      else if (val2 == 0) {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      } else {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      }
    } else if (op == "eq") {
      if (val1 == val2) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      } else {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
    } else if (op == "le") {
      if (val1 <= val2) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      } else {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
    } else if (op == "lt") {
      if (val1 < val2) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      } else {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
    } else if (op == "gt") {
      if (val1 > val2) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      } else {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
    } else if (op == "ge" ) {
      if (val1 >= val2) {
        block[i]["value"] = true;
        lvn_tb[i].op1 = "true";
      } else {
        block[i]["value"] = false;
        lvn_tb[i].op1 = "false";
      }
    }
    block[i]["op"] = "const";
    lvn_tb[i].op = "const";
    lvn_tb[i].op1_first = true;
  }
}

void modify_block(json &block, _lvn_tb &lvn_tb, _node_map &node_lookup,
                  _variable_map &variables) {
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

      // constant propogation
      do_constant_propogation(block, lvn_tb, variables, i);
    } else {
      // if op const do nothing
      if (block[i]["op"] == "id") {
        block[i]["dest"] = lvn_tb[i].var;
        get_arg1(block, lvn_tb, i);

        // print_lvn_node(lvn_tb[i]);

        // constant propogation
        do_constant_propogation(block, lvn_tb, variables, i);

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

        do_constant_folding(block, lvn_tb, variables, i);
        // std::cerr << "INST: \n" << block[i].dump(2) << std::endl;
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
  // std::ifstream file("commute.json");
  // json program = json::parse(file);

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
