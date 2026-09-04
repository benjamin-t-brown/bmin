#include "TestHarness.h"
#include <cstddef>

#ifdef BMIN_TEST_MODULES
import bmin.containers;
#else
#include "All.h"
#endif

SUITE(test_stringstream) {
  bmin::StringStream ss;
  ss << "port=" << 8080 << " ok=" << true << " name=" << bmin::String("srv");
  CHECK_EQ(ss.str(), "port=8080 ok=true name=srv");

  ss.clear();
  ss << static_cast<std::size_t>(42);
  CHECK_EQ(ss.str(), "42");

  ss.clear();
  ss << 1000000000u;
  CHECK_EQ(ss.str(), "1000000000");

  ss.clear();
  ss << 2.5f << "," << 3.25;
  CHECK_EQ(ss.str(), "2.5,3.25");
}
