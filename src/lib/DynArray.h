#pragma once

#include "String.h"

#include "internal/Types.h" // IWYU pragma: keep

namespace bmin {

template <typename T>
class DynArray {
  T* data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

  void reallocate(size_t newCap);

public:
  DynArray();
  explicit DynArray(size_t count);
  DynArray(const DynArray& o);
  DynArray(DynArray&& o) noexcept;
  ~DynArray();

  DynArray& operator=(DynArray o);

  size_t size() const {
    return size_;
  }

  size_t capacity() const {
    return capacity_;
  }

  bool empty() const {
    return size_ == 0;
  }

  T* data() {
    return data_;
  }

  const T* data() const {
    return data_;
  }

  void reserve(size_t n);
  void clear();

  void push_back(const T& value);
  void push_back(T&& value);

  template <typename... Args>
  void emplace_back(Args&&... args);

  void pop_back();

  T& operator[](size_t i) {
    return data_[i];
  }

  const T& operator[](size_t i) const {
    return data_[i];
  }

  T& at(size_t i);
  const T& at(size_t i) const;

  T* begin() {
    return data_;
  }

  T* end() {
    return data_ + size_;
  }

  const T* begin() const {
    return data_;
  }

  const T* end() const {
    return data_ + size_;
  }
};

extern template class DynArray<int>;
extern template class DynArray<String>;
extern template class DynArray<const char*>;

}  // namespace bmin

#include "DynArray.hpp"  // IWYU pragma: keep
