#include "bench_workers.h"

#include <vector>

void bench_worker1() {
  std::vector<int> values;
  for (int i = 0; i < 128; ++i)
    values.push_back(i * 3);
  values.reserve(512);
  values.pop_back();
}
