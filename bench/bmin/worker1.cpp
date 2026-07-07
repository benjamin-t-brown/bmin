#include "bench_workers.h"

#include "DynArray.h"
#include "Map.h"
#include "String.h"
#include "UniquePtr.h"

void bench_worker1() {
  bmin::DynArray<int> values;
  for (int i = 0; i < 128; ++i)
    values.push_back(i * 3);
  values.reserve(512);
  values.pop_back();
}
