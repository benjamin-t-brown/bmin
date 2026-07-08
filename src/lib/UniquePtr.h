#pragma once

#include "internal/Policy.h"
#include "internal/Utility.h"

namespace bmin {

class String;

template <typename T>
struct DefaultDelete {
  void operator()(T* p) const {
    delete p;
  }
};

template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {
  T* _ptr = nullptr;
  Deleter _deleter{};

public:
  using pointer = T*;
  using elementType = T;
  using deleterType = Deleter;

  UniquePtr() = default;

  explicit UniquePtr(T* p) : _ptr(p) {}

  UniquePtr(T* p, Deleter d) : _ptr(p), _deleter(bmin::move(d)) {}

  UniquePtr(UniquePtr&& o) noexcept
      : _ptr(o._ptr), _deleter(bmin::move(o._deleter)) {
    o._ptr = nullptr;
  }

  UniquePtr& operator=(UniquePtr&& o) noexcept {
    if (this != &o) {
      reset();
      _ptr = o._ptr;
      _deleter = bmin::move(o._deleter);
      o._ptr = nullptr;
    }
    return *this;
  }

  UniquePtr(const UniquePtr&) = delete;
  UniquePtr& operator=(const UniquePtr&) = delete;

  ~UniquePtr() {
    reset();
  }

  T* get() const {
    return _ptr;
  }

  Deleter& getDeleter() {
    return _deleter;
  }

  const Deleter& getDeleter() const {
    return _deleter;
  }

  T* release() {
    T* p = _ptr;
    _ptr = nullptr;
    return p;
  }

  void reset(T* p = nullptr) {
    if (_ptr) {
      _deleter(_ptr);
    }
    _ptr = p;
  }

  void swap(UniquePtr& o) noexcept {
    bmin::detail::swap(_ptr, o._ptr);
    bmin::detail::swap(_deleter, o._deleter);
  }

  T& operator*() const {
    BMIN_ASSERT(_ptr);
    return *_ptr;
  }

  T* operator->() const {
    BMIN_ASSERT(_ptr);
    return _ptr;
  }

  explicit operator bool() const {
    return _ptr != nullptr;
  }
};

template <typename T, typename Deleter>
void swap(UniquePtr<T, Deleter>& a, UniquePtr<T, Deleter>& b) noexcept {
  a.swap(b);
}

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
  return UniquePtr<T>(new T(bmin::forward<Args>(args)...));
}

extern template class UniquePtr<int>;
extern template class UniquePtr<String>;

}  // namespace bmin
