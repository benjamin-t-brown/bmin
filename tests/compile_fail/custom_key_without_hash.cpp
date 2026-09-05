// Negative compile test: custom structures must define a logical hash.
#ifdef BMIN_TEST_MODULES
import bmin.containers;
#else
#include "Map.h"
#endif

struct CustomKey {
  char category;
  int id;

  bool operator==(const CustomKey& o) const {
    return category == o.category && id == o.id;
  }
};

int main() {
  bmin::Map<CustomKey, int> values;
  values.insert(CustomKey{'a', 1}, 10);
  return 0;
}
