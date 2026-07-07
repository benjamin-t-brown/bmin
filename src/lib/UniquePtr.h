#pragma once

#include "internal/Policy.h"
#include "internal/Utility.h"

namespace bmin {

class String;

template <typename T>
class UniquePtr {
  T* ptr_ = nullptr;

public:
  UniquePtr() = default;

  explicit UniquePtr(T* p) : ptr_(p) {}

  UniquePtr(UniquePtr&& o) noexcept : ptr_(o.ptr_) {
    o.ptr_ = nullptr;
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
    return ptr_;
  }

  T* release() {
    T* p = ptr_;
    ptr_ = nullptr;
    return p;
  }

  void reset(T* p = nullptr) {
    if (ptr_)
      delete ptr_;
    ptr_ = p;
  }

  T& operator*() const {
    BMIN_ASSERT(ptr_);
    return *ptr_;
  }

  T* operator->() const {
    BMIN_ASSERT(ptr_);
    return ptr_;
  }

  explicit operator bool() const {
    return ptr_ != nullptr;
  }
};

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
  return UniquePtr<T>(new T(bmin::forward<Args>(args)...));
}

extern template class UniquePtr<int>;
extern template class UniquePtr<String>;

}  // namespace bmin
