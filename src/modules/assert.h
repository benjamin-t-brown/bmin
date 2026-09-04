#pragma once

// Macro-only — modules cannot export macros. Include from a global module
// fragment when a module TU needs BMIN_ASSERT. Relies on the reachable
// template support exported by `bmin.core`.

#ifndef NDEBUG
#define BMIN_ASSERT(cond)                                       \
  do {                                                          \
    if (!(cond)) {                                              \
      ::bmin::detail::assertFailed(#cond, __FILE__, __LINE__);  \
    }                                                           \
  } while (0)
#else
#define BMIN_ASSERT(cond) ((void)0)
#endif
