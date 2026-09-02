#include "bench_workers.h"


import bmin.containers;

void bench_worker1() {
  bmin::DynArray<int> values;
  for (int i = 0; i < 128; ++i) {
    values.pushBack(i * 3);
  }
  values.reserve(512);
  values.popBack();
}
