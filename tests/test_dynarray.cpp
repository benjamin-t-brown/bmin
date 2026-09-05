#include "TestHarness.h"
#include <cstdint>

#ifdef BMIN_TEST_MODULES
import bmin.containers;
#else
#include "All.h"
#endif

namespace {

struct alignas(64) AlignedValue {
  int value = 0;

  explicit AlignedValue(int valueA = 0) : value(valueA) {}

  bool operator==(const AlignedValue& o) const {
    return value == o.value;
  }
};

}  // namespace

SUITE(test_dynarray) {
  bmin::DynArray<int> a;
  a.pushBack(1);
  a.pushBack(2);
  a.pushBack(3);
  CHECK_EQ(a.size(), 3u);
  CHECK_EQ(a[0], 1);
  CHECK_EQ(a.at(2), 3);

  a.popBack();
  CHECK_EQ(a.size(), 2u);

  bmin::DynArray<int> b(a);
  CHECK_EQ(b[1], 2);

  bmin::DynArray<int> c(bmin::move(b));
  CHECK_EQ(c.size(), 2u);
  CHECK(b.empty());

  c.reserve(100);
  CHECK(c.capacity() >= 100u);

  CHECK(c.contains(1));
  CHECK(c.contains(2));
  CHECK(!c.contains(3));

  bmin::DynArray<bmin::String> words = {
      bmin::String("drop-one"), bmin::String("keep-one"),
      bmin::String("drop-two"), bmin::String("keep-two")};
  const std::size_t removed = words.eraseIf(
      [](const bmin::String& word) { return word.startsWith("drop"); });
  CHECK_EQ(removed, 2u);
  CHECK_EQ(words.size(), 2u);
  CHECK_EQ(words[0], "keep-one");
  CHECK_EQ(words[1], "keep-two");

  bmin::DynArray<AlignedValue> aligned;
  aligned.emplaceBack(7);
  aligned.emplaceBack(9);
  CHECK_EQ(reinterpret_cast<std::uintptr_t>(aligned.data()) %
               alignof(AlignedValue),
           0u);
  CHECK_EQ(aligned.at(1).value, 9);
}
