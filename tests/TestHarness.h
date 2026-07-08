#pragma once

#include <stdio.h>  // IWYU pragma: keep

inline int& testFailures() {
  static int n = 0;
  return n;
}

inline void check(bool ok, const char* expr, const char* file, int line) {
  if (!ok)
    fprintf(stderr, "  FAIL %s:%d: %s\n", file, line, expr);
  testFailures() += !ok;
}

#define CHECK(expr) check((expr), #expr, __FILE__, __LINE__)
#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_NE(a, b) CHECK((a) != (b))

#define SUITE(name) void name()
#define RUN(suite)                  \
  do {                              \
    fprintf(stderr, "SUITE %s\n", #suite); \
    suite();                        \
  } while (0)
