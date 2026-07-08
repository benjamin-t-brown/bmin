#include "TestHarness.h"  // IWYU pragma: keep

#include "All.h"  // IWYU pragma: keep

struct Foo {
  double value;
  int count;
  bool flag;
  bmin::String name;

  Foo(double v, int c, bool f, bmin::String n)
      : value(v), count(c), flag(f), name(bmin::move(n)) {}
};

SUITE(test_uniqueptr) {
  bmin::UniquePtr<Foo> p =
      bmin::makeUnique<Foo>(3.14, 7, true, bmin::String("widget"));
  CHECK(static_cast<bool>(p));
  CHECK_EQ(p->value, 3.14);
  CHECK_EQ(p->count, 7);
  CHECK(p->flag);
  CHECK_EQ(p->name, "widget");

  bmin::UniquePtr<Foo> q(bmin::move(p));
  CHECK(!static_cast<bool>(p));
  CHECK(static_cast<bool>(q));
  CHECK_EQ(q->value, 3.14);
  CHECK_EQ(q->name, "widget");

  q->count = 99;
  CHECK_EQ(q->count, 99);

  q.reset();
  CHECK(!static_cast<bool>(q));
}
