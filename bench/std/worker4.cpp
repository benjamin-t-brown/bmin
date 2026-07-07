#include "bench_workers.h"

#include <memory>
#include <vector>

void bench_worker4() {
  std::unique_ptr<int> value = std::make_unique<int>(42);
  std::vector<std::unique_ptr<int>> owned;
  for (int i = 0; i < 16; ++i)
    owned.push_back(std::make_unique<int>(i));
  value.reset(new int(99));
  (void)value.get();
}
