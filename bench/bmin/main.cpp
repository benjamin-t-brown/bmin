#include "bench_workers.h"

#include <stdio.h>

int main() {
  int code = run_bench_workers();
  fprintf(stderr, "bmin bench done\n");
  return code;
}
