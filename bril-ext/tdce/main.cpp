#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "../json.hpp"

using json = nlohmann::json;

struct Node {
  bool status;
  int idx;
};


void optimize_function(json &f) {
  // Assuming only one function in a program
  // Ordered map because we want to delete
  bool changed = true;
  std::unordered_map<std::string, Node> tb;
  std::vector<int> delete_idx;
  int idx;

  while(changed) {
    changed = false;
    delete_idx.clear();
    tb.clear();
    idx = 0;

    for (auto inst : f["instrs"]) {
      if (inst.contains("dest")) {
        auto key = inst["dest"];

        if(tb.find(key) != tb.end()) {
          if(!tb[key].status) {
            // Reassignment without use
            delete_idx.push_back(tb[key].idx);  
          } 
            
          tb[key].idx = idx;
          tb[key].status = false;
        } else {
          tb.insert({key, {false, idx}});
        }
      }

      if (inst.contains("args")) {
        for (const auto arg : inst["args"]) {
          assert(tb.find(arg) != tb.end());
          tb[arg].status = true;
        }
      }

      if(inst["op"] == "br" || inst["op"] == "jmp") {
        for(auto &v: tb) {
          v.second.status = true;
        }
      }

      ++idx;
    }
    
    for(auto [key, n]: tb) {
      if(!n.status)
        delete_idx.push_back(n.idx);
    }

    std::sort(begin(delete_idx), end(delete_idx), std::greater<int>());

    for(auto i : delete_idx) {
      f["instrs"].erase(i);
      changed = true;
    }
  }
}

void eliminate_dead_code() {
  json program = json::parse(stdin);

  for(auto &f: program["functions"]) 
    optimize_function(f);

  std::cout << program.dump();
}

int main() {

  eliminate_dead_code();

  return 0;
}
