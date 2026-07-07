#include "bench_workers.h"

#include "Map.h"
#include "String.h"

void bench_worker3() {
  bmin::Map<bmin::String, int> table;
  table.insert("one", 1);
  table.insert("two", 2);
  table.insert("three", 3);
  table["two"] = 22;
  table.erase("one");
  (void)table.contains("three");
}
