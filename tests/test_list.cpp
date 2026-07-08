#include "TestHarness.h"  // IWYU pragma: keep

#include "List.h"  // IWYU pragma: keep

SUITE(test_list) {
  bmin::List<int> list;
  CHECK(list.empty());

  list.push_back(1);
  list.push_front(0);
  list.push_back(2);
  CHECK_EQ(list.size(), 3u);

  list.pop_front();
  CHECK_EQ(list.size(), 2u);

  int sum = 0;
  for (int v : list)
    sum += v;
  CHECK_EQ(sum, 3);

  list.pop_back();
  CHECK_EQ(list.size(), 1u);
  CHECK_EQ(*list.begin(), 1);

  bmin::List<bmin::String> names;
  names.push_back(bmin::String("ann"));
  names.push_back(bmin::String("bob"));
  CHECK_EQ((*names.begin()), "ann");
}
