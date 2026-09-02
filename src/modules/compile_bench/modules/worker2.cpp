#include "bench_workers.h"


import bmin.containers;

void bench_worker2() {
  bmin::String text = "alpha";
  text.append("-beta-gamma");
  text.append(text);
  bmin::DynArray<bmin::String> parts;
  parts.pushBack(text.slice(0, 5));
  parts.pushBack(text.slice(5));
}
