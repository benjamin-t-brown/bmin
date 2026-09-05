#pragma once

#include "./Policy.h"
#include "./Types.h" // IWYU pragma: keep
#include "./Utility.h"

#include <new>

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

template <typename T>
T* allocate(size_t count) {
  if (count == 0) {
    return nullptr;
  }
  if (count > static_cast<size_t>(-1) / sizeof(T)) {
    fatal();
  }
  const size_t bytes = count * sizeof(T);
  if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
    return static_cast<T*>(
        ::operator new(bytes, std::align_val_t(alignof(T))));
  }
  return static_cast<T*>(allocateRaw(bytes));
}

template <typename T>
void deallocate(T* p) noexcept {
  if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
    ::operator delete(p, std::align_val_t(alignof(T)));
  } else {
    deallocateRaw(p);
  }
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
