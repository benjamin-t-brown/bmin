#pragma once

#include "./Policy.h"
#include "./Types.h" // IWYU pragma: keep
#include "./Utility.h"

// Minimal replacements for <new> (global allocators are provided by libc++ / libstdc++).
void* operator new(size_t bytes);
void operator delete(void* p) noexcept;

#if defined(__has_include) && __has_include(<new>)
#include <new>
#else
inline void* operator new(size_t, void* p) noexcept { return p; }
#endif

namespace bmin {
namespace storage {

inline void* allocateRaw(size_t bytes) {
  if (bytes == 0) {
    return nullptr;
  }
  void* p = ::operator new(bytes);
  if (!p) {
    fatal();
  }
  return p;
}

inline void deallocateRaw(void* p) noexcept {
  ::operator delete(p);
}

template <typename T, typename... Args>
T* constructAt(T* p, Args&&... args) {
  return new (p) T(bmin::forward<Args>(args)...);
}

template <typename T>
void destroyAt(T* p) {
  p->~T();
}

}  // namespace storage
}  // namespace bmin
