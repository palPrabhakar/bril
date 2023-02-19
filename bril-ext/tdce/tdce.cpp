#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
  json program = json::parse(stdin);

  std::cout<<"Input Program\n";
  std::cout<<program.dump(2)<<std::endl;

  return 0;
}
