#include "TestHarness.h"  // IWYU pragma: keep

SUITE(test_string);
SUITE(test_dynarray);
SUITE(test_map);
SUITE(test_uniqueptr);
SUITE(test_stringinterop);
SUITE(test_custom_types);

SUITE(test_queue);
SUITE(test_list);
SUITE(test_stringstream);

int main() {
  RUN(test_string);
  RUN(test_dynarray);
  RUN(test_map);
  RUN(test_uniqueptr);
  RUN(test_stringinterop);
  RUN(test_custom_types);
  RUN(test_queue);
  RUN(test_list);
  RUN(test_stringstream);

  if (testFailures() != 0) {
    fprintf(stderr, "%d CHECK(s) failed\n", testFailures());
    return 1;
  }
  fprintf(stderr, "ALL PASS\n");
  return 0;
}
