#include "TestHarness.h"  // IWYU pragma: keep

#include "All.h"  // IWYU pragma: keep

namespace {

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

  for (int i = 0; i < 20; ++i)
    m.insert(keyFor(i), i);

  for (int i = 0; i < 20; ++i)
    CHECK_EQ(m.find(keyFor(i))->value, i);

  CHECK(m.erase(bmin::String("one")));
  CHECK(!m.contains(bmin::String("one")));
}
