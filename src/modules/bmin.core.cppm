module;
#include <cstddef>
#include <new>

export module bmin.core;

// Placement new must be visible where constructAt and the container templates
// are instantiated in consumer translation units.
export {
  using ::operator new;
  using ::operator delete;
}

export namespace bmin {

// These utility functions are supported public API. The nested detail and
// storage namespaces below are reachable template support, not public API.
template <typename T>
constexpr T&& move(T& value) noexcept {
  return static_cast<T&&>(value);
}

template <typename T>
constexpr T&& forward(T& value) noexcept {
  return static_cast<T&&>(value);
}

template <typename T, typename U = T>
T exchange(T& object, U&& newValue) {
  T old = move(object);
  object = forward<U>(newValue);
  return old;
}

namespace detail {

[[noreturn]] void fatal();

#ifndef NDEBUG
[[noreturn]] void assertFailed(const char* expression,
                               const char* file,
                               int line);
#endif

template <typename T>
void swap(T& a, T& b) noexcept {
  T temporary = move(a);
  a = move(b);
  b = move(temporary);
}

namespace storage {

inline void* allocateRaw(std::size_t bytes) {
  if (bytes == 0) {
    return nullptr;
  }
  void* allocation = ::operator new(bytes);
  if (!allocation) {
    fatal();
  }
  return allocation;
}

inline void deallocateRaw(void* allocation) noexcept {
  ::operator delete(allocation);
}

template <typename T, typename... Args>
T* constructAt(T* location, Args&&... args) {
  return new (location) T(bmin::forward<Args>(args)...);
}

template <typename T>
void destroyAt(T* object) {
  object->~T();
}

}  // namespace storage
}  // namespace detail
}  // namespace bmin
