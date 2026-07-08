#pragma once

namespace bmin {

[[noreturn]] void fatal();

#ifndef NDEBUG
[[noreturn]] void assertFailed(const char* expr, const char* file, int line);
#endif

}  // namespace bmin

#ifndef NDEBUG
#define BMIN_ASSERT(cond)                                       \
  do {                                                          \
    if (!(cond)) {                                              \
      ::bmin::assertFailed(#cond, __FILE__, __LINE__);          \
    }                                                           \
  } while (0)
#else
#define BMIN_ASSERT(cond) ((void)0)
#endif
