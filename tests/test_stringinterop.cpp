#include "TestHarness.h"  // IWYU pragma: keep

#include "StringInterop.h"  // IWYU pragma: keep

SUITE(test_stringinterop) {
  bmin::String s("interop");
  std::string_view sv = bmin::toStringView(s);
  CHECK_EQ(sv, std::string_view("interop"));
  CHECK(s == std::string_view("interop"));
}
