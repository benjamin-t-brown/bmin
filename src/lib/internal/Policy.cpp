#include "Policy.h"

#include <stdlib.h>

namespace bmin {

void fatal() { abort(); }

#ifndef NDEBUG

#include <stdio.h>

void assertFailed(const char* expr, const char* file, int line) {
  fprintf(stderr, "BMIN_ASSERT failed: %s at %s:%d\n", expr, file, line);
  abort();
}

#endif

}  // namespace bmin
