#include "TestHarness.h"

#include "StringInterop.h"

SUITE(test_stringinterop) {
  bmin::String s("interop");
  std::string_view sv = bmin::toStringView(s);
  CHECK_EQ(sv, std::string_view("interop"));
  CHECK(s == std::string_view("interop"));
}
