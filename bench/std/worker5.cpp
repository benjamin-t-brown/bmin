#include "bench_workers.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::string join(const std::string& sep, const std::vector<std::string>& parts) {
  if (parts.empty()) {
    return {};
  }
  std::string out = parts[0];
  for (size_t i = 1; i < parts.size(); ++i) {
    out.append(sep);
    out.append(parts[i]);
  }
  return out;
}

}  // namespace

void bench_worker5() {
  std::unordered_map<std::string, std::string> labels;
  labels.emplace("host", "localhost");
  labels.emplace("port", "8080");
  std::vector<std::string> parts;
  parts.push_back(labels["host"]);
  parts.push_back(labels["port"]);
  std::string joined = join(",", parts);
  (void)joined.c_str();
}
