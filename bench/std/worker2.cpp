#include "bench_workers.h"

#include <string>
#include <vector>

void bench_worker2() {
  std::string text = "alpha";
  text.append("-beta-gamma");
  text.append(text);
  std::vector<std::string> parts;
  parts.push_back(text.substr(0, 5));
  parts.push_back(text.substr(5));
}
