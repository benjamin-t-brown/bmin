#include "TestHarness.h"  // IWYU pragma: keep
#include "List.h" 

SUITE(test_list) {
  bmin::List<int> list;
  CHECK(list.empty());

  list.pushBack(1);
  list.pushFront(0);
  list.pushBack(2);
  CHECK_EQ(list.size(), 3u);

  list.popFront();
  CHECK_EQ(list.size(), 2u);

  int sum = 0;
  for (int v : list) {
    sum += v;
  }
  CHECK_EQ(sum, 3);

  list.popBack();
  CHECK_EQ(list.size(), 1u);
  CHECK_EQ(*list.begin(), 1);

  bmin::List<bmin::String> names;
  names.pushBack(bmin::String("ann"));
  names.pushBack(bmin::String("bob"));
  CHECK_EQ((*names.begin()), "ann");

  CHECK(list.contains(1));
  CHECK(!list.contains(99));
  CHECK(names.contains(bmin::String("bob")));
  CHECK(!names.contains(bmin::String("zed")));

  bmin::List<int> fromInit = {10, 20, 30};
  CHECK_EQ(fromInit.size(), 3u);
  CHECK(fromInit.contains(10));
  CHECK(fromInit.contains(20));
  CHECK(fromInit.contains(30));
  CHECK(!fromInit.contains(40));
}
