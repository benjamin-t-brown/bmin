module;
#include <stdlib.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

module bmin.core;

namespace bmin::detail {

void fatal() {
  abort();
}

#ifndef NDEBUG
void assertFailed(const char* expression, const char* file, int line) {
  fprintf(stderr, "BMIN_ASSERT failed: %s at %s:%d\n", expression, file,
          line);
  abort();
}
#endif

}  // namespace bmin::detail
