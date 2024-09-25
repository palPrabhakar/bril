#include "df.h"

// Do dominator analysis
void do_dom_analysis() {
  json program = json::parse(stdin);

  // std::ifstream file("loopcond.json");
  // json program = json::parse(file);

  for (auto &f : program["functions"]) {
    // auto doms = find_dominators(f);
    // auto idoms = create_dominator_tree(f);
    auto df = find_dominance_frontier(f);
  }

}
int main() {
  // Reaching definitions
  do_dom_analysis();

  return 0;
}
