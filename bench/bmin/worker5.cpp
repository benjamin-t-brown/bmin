#include "bench_workers.h"

#include "DynArray.h"
#include "Map.h"
#include "String.h"

void bench_worker5() {
  bmin::Map<bmin::String, bmin::String> labels;
  labels.insert("host", "localhost");
  labels.insert("port", "8080");
  bmin::DynArray<bmin::String> parts;
  parts.push_back(labels["host"]);
  parts.push_back(labels["port"]);
  bmin::String joined = bmin::String::join(",", parts);
  (void)joined.c_str();
}
