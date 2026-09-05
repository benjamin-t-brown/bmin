#include "TestHarness.h"
#include <string_view>
#include <type_traits>

#ifdef BMIN_TEST_MODULES
import bmin.containers;
#else
#include "All.h"
#endif

namespace {

struct SeededHash {
  std::size_t seed = 0;

  std::size_t operator()(int value) const {
    return static_cast<std::size_t>(value) ^ seed;
  }
};

struct EnabledEqual {
  bool enabled = false;

  bool operator()(int a, int b) const {
    return enabled && a == b;
  }
};

bmin::String keyFor(int i) {
  bmin::String s("k");
  char ch[2] = {static_cast<char>('a' + (i % 26)), '\0'};
  s.append(ch);
  if (i >= 26) {
    char tens[2] = {static_cast<char>('0' + (i / 26)), '\0'};
    s.append(tens);
  }
  return s;
}

}  // namespace

SUITE(test_map) {
  bmin::Map<bmin::String, int> m;
  CHECK(m.insert(bmin::String("one"), 1));
  CHECK(!m.insert(bmin::String("one"), 99));
  CHECK_EQ(m.find(bmin::String("one"))->value, 99);

  m[bmin::String("two")] = 2;
  CHECK(m.contains(bmin::String("two")));
  CHECK_EQ(m.size(), 2u);

  for (int i = 0; i < 20; ++i) {
    m.insert(keyFor(i), i);
  }

  for (int i = 0; i < 20; ++i) {
    CHECK_EQ(m.find(keyFor(i))->value, i);
  }

  CHECK(m.erase(bmin::String("one")));
  CHECK(!m.contains(bmin::String("one")));

  const bmin::Map<bmin::String, int>& cm = m;
  static_assert(
      !std::is_assignable_v<decltype((m.begin()->key)), bmin::String>);
  static_assert(std::is_assignable_v<decltype((m.begin()->value)), int>);
  static_assert(
      !std::is_assignable_v<decltype((cm.begin()->value)), int>);
  size_t n = 0;
  for (auto it = cm.begin(); it != cm.end(); ++it) {
    ++n;
  }
  CHECK_EQ(n, cm.size());

  const std::string_view two("two");
  CHECK(m.contains(two));
  CHECK(m.contains("two"));
  CHECK_EQ(m.find(two)->value, 2);
  CHECK_EQ(cm.find(two)->value, 2);
  CHECK(m.erase(two));
  CHECK(!m.contains(two));

  m.reserve(100);
  m.insert(bmin::String("after-reserve"), 7);
  CHECK_EQ(m.find(bmin::String("after-reserve"))->value, 7);
  m.clear();
  CHECK(m.empty());
  CHECK_EQ(m.size(), 0u);
  m.insert(bmin::String("after-clear"), 8);
  CHECK_EQ(m.find(bmin::String("after-clear"))->value, 8);

  using StatefulMap = bmin::Map<int, bmin::String, SeededHash, EnabledEqual>;
  StatefulMap stateful(SeededHash{0x5a}, EnabledEqual{true});
  stateful.insert(11, bmin::String("eleven"));

  StatefulMap copied(stateful);
  CHECK(copied.contains(11));

  StatefulMap moved(bmin::move(stateful));
  CHECK(moved.contains(11));

  StatefulMap assigned(SeededHash{0xff}, EnabledEqual{false});
  assigned = moved;
  CHECK(assigned.contains(11));
}
