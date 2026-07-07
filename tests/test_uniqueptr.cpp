#include "TestHarness.h"

#include "All.h"

SUITE(test_uniqueptr) {
  bmin::UniquePtr<int> p = bmin::makeUnique<int>(42);
  CHECK_EQ(*p, 42);

  bmin::UniquePtr<int> q(bmin::move(p));
  CHECK(!p);
  CHECK_EQ(*q, 42);

  q.reset();
  CHECK(!q);
}
