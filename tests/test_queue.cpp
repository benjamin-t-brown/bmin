#include "TestHarness.h"

#ifdef BMIN_TEST_MODULES
import bmin.containers;
#else
#include "All.h"
#endif

namespace {

struct QueueTracked {
  static int alive;
  int value = 0;

  explicit QueueTracked(int valueA = 0) : value(valueA) {
    ++alive;
  }

  QueueTracked(const QueueTracked& o) : value(o.value) {
    ++alive;
  }

  QueueTracked(QueueTracked&& o) noexcept : value(o.value) {
    ++alive;
    o.value = -1;
  }

  QueueTracked& operator=(const QueueTracked&) = default;
  QueueTracked& operator=(QueueTracked&&) = default;

  ~QueueTracked() {
    --alive;
  }
};

int QueueTracked::alive = 0;

}  // namespace

SUITE(test_queue) {
  bmin::Queue<int> q;
  CHECK(q.empty());

  q.push(1);
  q.push(2);
  q.push(3);
  CHECK_EQ(q.size(), 3u);
  CHECK_EQ(q.front(), 1);
  CHECK_EQ(q.back(), 3);

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

  {
    bmin::Queue<QueueTracked> tracked;
    for (int i = 0; i < 10; ++i) {
      tracked.emplace(i);
    }
    CHECK_EQ(tracked.front().value, 0);
    CHECK_EQ(tracked.back().value, 9);
    tracked.pop();
    tracked.pop();
    tracked.pop();
    CHECK_EQ(tracked.front().value, 3);

    bmin::Queue<QueueTracked> copied(tracked);
    CHECK_EQ(copied.size(), 7u);
    CHECK_EQ(copied.front().value, 3);
    CHECK_EQ(copied.back().value, 9);

    bmin::Queue<QueueTracked> moved(bmin::move(copied));
    CHECK(copied.empty());
    CHECK_EQ(moved.size(), 7u);
    moved.clear();
    CHECK(moved.empty());
  }
  CHECK_EQ(QueueTracked::alive, 0);
}
