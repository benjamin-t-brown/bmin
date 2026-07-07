#include <string_view>

#include "StringInterop.h"

int main() {
  std::string_view v =
      bmin::toStringView(bmin::String("x").slice(0, 1));
  (void)v;
  return 0;
}
