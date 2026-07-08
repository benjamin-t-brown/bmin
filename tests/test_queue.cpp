#include "TestHarness.h"  // IWYU pragma: keep

#include "Queue.h"  // IWYU pragma: keep

SUITE(test_queue) {
  bmin::Queue<int> q;
  CHECK(q.empty());

  q.push(1);
  q.push(2);
  q.push(3);
  CHECK_EQ(q.size(), 3u);
  CHECK_EQ(q.front(), 1);

  q.pop();
  CHECK_EQ(q.front(), 2);

  q.pop();
  q.pop();
  CHECK(q.empty());

  for (int i = 0; i < 20; ++i) {
    q.push(i);
  }
  CHECK_EQ(q.size(), 20u);
  CHECK_EQ(q.front(), 0);

  q.clear();
  CHECK(q.empty());

  bmin::Queue<bmin::String> words;
  words.push(bmin::String("a"));
  words.push(bmin::String("b"));
  CHECK_EQ(words.front(), "a");
  words.pop();
  CHECK_EQ(words.front(), "b");
}
