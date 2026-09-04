#include "TestHarness.h"
#include <string_view>

#ifdef BMIN_TEST_MODULES
import bmin.string_interop;
#else
#include "StringInterop.h"
#endif

SUITE(test_stringinterop) {
  bmin::String s("interop");
  std::string_view sv = bmin::toStringView(s);
  CHECK_EQ(sv, std::string_view("interop"));
  CHECK(s == std::string_view("interop"));
  CHECK(std::string_view("interop") == s);
  CHECK(s != std::string_view("other"));
  CHECK(std::string_view("other") != s);

  bmin::String abc("abc");
  CHECK(abc < std::string_view("abd"));
  CHECK(std::string_view("abd") > abc);
  CHECK(abc <= std::string_view("abc"));
  CHECK(std::string_view("abc") >= abc);
}
