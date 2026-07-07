#include "bench_workers.h"

#include <string>
#include <unordered_map>

void bench_worker3() {
  std::unordered_map<std::string, int> table;
  table.emplace("one", 1);
  table.emplace("two", 2);
  table.emplace("three", 3);
  table["two"] = 22;
  table.erase("one");
  (void)table.contains("three");
}
