#pragma once

#include "internal/Policy.h"
#include "internal/Utility.h"

namespace bmin {

class String;

template <typename T>
class UniquePtr {
  T* _ptr = nullptr;

public:
  UniquePtr() = default;

  explicit UniquePtr(T* p) : _ptr(p) {}

  UniquePtr(UniquePtr&& o) noexcept : _ptr(o._ptr) {
    o._ptr = nullptr;
  }

  UniquePtr& operator=(UniquePtr&& o) noexcept {
    reset(o.release());
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

  T* release() {
    T* p = _ptr;
    _ptr = nullptr;
    return p;
  }

  void reset(T* p = nullptr) {
    if (_ptr) {
      delete _ptr;
    }
    _ptr = p;
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

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
  return UniquePtr<T>(new T(bmin::forward<Args>(args)...));
}

extern template class UniquePtr<int>;
extern template class UniquePtr<String>;

}  // namespace bmin
