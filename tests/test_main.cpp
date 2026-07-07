#include "TestHarness.h"

SUITE(test_string);
SUITE(test_dynarray);
SUITE(test_map);
SUITE(test_uniqueptr);
SUITE(test_stringinterop);
SUITE(test_custom_types);

int main() {
  RUN(test_string);
  RUN(test_dynarray);
  RUN(test_map);
  RUN(test_uniqueptr);
  RUN(test_stringinterop);
  RUN(test_custom_types);

  if (testFailures() != 0) {
    fprintf(stderr, "%d CHECK(s) failed\n", testFailures());
    return 1;
  }
  fprintf(stderr, "ALL PASS\n");
  return 0;
}
