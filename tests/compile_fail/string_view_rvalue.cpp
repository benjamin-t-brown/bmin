// Negative compile test: must fail (deleted toStringView(String&&)).
// clangd diagnostics suppressed via tests/compile_fail/.clangd
#include <string_view>  // IWYU pragma: keep

#include "StringInterop.h"  // IWYU pragma: keep

int main() {
  std::string_view v =
      bmin::toStringView(bmin::String("x").slice(0, 1));
  (void)v;
  return 0;
}
