module;
#include <cstddef>

export module bmin.types;

// Placeholder module so the Makefile BMI graph has a root node.
// Prefer std::size_t in module interfaces; do not export global size_t
// (conflicts with C library headers on MinGW).
export namespace bmin {
  using Size = std::size_t;
  using PtrDiff = std::ptrdiff_t;
}
