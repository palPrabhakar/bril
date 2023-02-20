#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void optimize_function(json &f) {
  // Assuming only one function in a program
  // Ordered map because we want to delete
  bool changed = true;
  std::map<std::string, bool> tb;

  while(changed) {
    changed = false;
    tb.clear();
    for (auto inst : f["instrs"]) {

      if (inst.contains("dest")) {
        // std::cout<<inst["dest"]<<std::endl;
        tb.insert({inst["dest"], false});
      }

      if (inst.contains("args")) {
        for (const auto arg : inst["args"]) {
          assert(tb.find(arg) != tb.end());
          tb[arg] = true;
        }
      }
    }

    for (auto it = begin(f["instrs"]); it != end(f["instrs"]); ++it) {
      if (it->contains("dest") && !tb[(*it)["dest"]]) {
        f["instrs"].erase(it);
        changed = true;
      }
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
