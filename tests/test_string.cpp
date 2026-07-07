#include "TestHarness.h"

#include "All.h"

SUITE(test_string) {
  bmin::String a("hello");
  a.append(" world");
  CHECK_EQ(a, "hello world");

  bmin::String mid = a.slice(6, 11);
  CHECK_EQ(mid, "world");

  bmin::String tail = a.slice(6);
  CHECK_EQ(tail, "world");

  bmin::String empty = a.slice(100, 200);
  CHECK(empty.empty());

  bmin::String parts[3] = {bmin::String("a"), bmin::String("b"),
                             bmin::String("c")};
  bmin::String joined = bmin::String::join("-", parts, 3);
  CHECK_EQ(joined, "a-b-c");

  bmin::DynArray<bmin::String> arr;
  arr.push_back(bmin::String("x"));
  arr.push_back(bmin::String("y"));
  bmin::String joined2 = bmin::String::join(",", arr);
  CHECK_EQ(joined2, "x,y");

  bmin::String moved = bmin::move(a);
  CHECK(moved.size() > 0);
  CHECK(a.empty());
}
