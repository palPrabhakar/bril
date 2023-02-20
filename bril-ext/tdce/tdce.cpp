#include <iostream>
#include <nlohmann/json.hpp>
#include <map>

using json = nlohmann::json;

void eliminate_dead_code() {
  json program = json::parse(stdin);
  
  // Assuming only one function in a program
  // Ordered map because we want to delete
  std::map<std::string, bool> tb;

  auto &f = program["functions"][0];
  for(auto inst : f["instrs"]) {

    if(inst.contains("dest")) {
      // std::cout<<inst["dest"]<<std::endl;
      tb.insert({inst["dest"], false});
    }

    if (inst.contains("args")) {
      for(const auto arg: inst["args"]) {
        assert(tb.find(arg) != tb.end());
        tb[arg] = true;
      }
    }
  }

  
  for(auto it = begin(f["instrs"]); it != end(f["instrs"]); ++it)  {
    if(it->contains("dest") && !tb[(*it)["dest"]]) 
      f["instrs"].erase(it);
  }

  std::cout<<program.dump();

}

int main() {

  eliminate_dead_code();

  return 0;
}
