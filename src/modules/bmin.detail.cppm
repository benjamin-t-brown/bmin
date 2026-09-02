module;
#include <new>
#include <cstddef>

export module bmin.detail;

// Placement new must be visible where constructAt / DynArray are instantiated
// in consumer TUs. GMF #includes are not, so re-export the operators.
export {
  using ::operator new;
  using ::operator delete;
}

export namespace bmin {

[[noreturn]] void fatal();

#ifndef NDEBUG
[[noreturn]] void assertFailed(const char* expr, const char* file, int line);
#endif

template <typename T>
constexpr T&& move(T& t) noexcept {
  return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(T& t) noexcept {
  return static_cast<T&&>(t);
}

namespace detail {

template <typename T>
void swap(T& a, T& b) noexcept {
  T tmp = move(a);
  a = move(b);
  b = move(tmp);
}

}  // namespace detail

template <typename T, typename U = T>
T exchange(T& obj, U&& newValue) {
  T old = move(obj);
  obj = forward<U>(newValue);
  return old;
}

namespace storage {

inline void* allocateRaw(std::size_t bytes) {
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
