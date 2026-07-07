#include "TestHarness.h"

#include "All.h"

SUITE(test_dynarray) {
  bmin::DynArray<int> a;
  a.push_back(1);
  a.push_back(2);
  a.push_back(3);
  CHECK_EQ(a.size(), 3u);
  CHECK_EQ(a[0], 1);
  CHECK_EQ(a.at(2), 3);

  a.pop_back();
  CHECK_EQ(a.size(), 2u);

  bmin::DynArray<int> b(a);
  CHECK_EQ(b[1], 2);

  bmin::DynArray<int> c(bmin::move(b));
  CHECK_EQ(c.size(), 2u);
  CHECK(b.empty());

  c.reserve(100);
  CHECK(c.capacity() >= 100u);
}
