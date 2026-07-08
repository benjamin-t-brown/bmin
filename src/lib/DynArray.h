#pragma once

#include "String.h"

#include "internal/Types.h" // IWYU pragma: keep

namespace bmin {

template <typename T>
class DynArray {
  T* _data = nullptr;
  size_t _size = 0;
  size_t _capacity = 0;

  void reallocate(size_t newCap);

public:
  DynArray();
  explicit DynArray(size_t count);
  DynArray(const DynArray& o);
  DynArray(DynArray&& o) noexcept;
  ~DynArray();

  DynArray& operator=(DynArray o);

  size_t size() const {
    return _size;
  }

  size_t capacity() const {
    return _capacity;
  }

  bool empty() const {
    return _size == 0;
  }

  T* data() {
    return _data;
  }

  const T* data() const {
    return _data;
  }

  void reserve(size_t n);
  void clear();

  void pushBack(const T& value);
  void pushBack(T&& value);

  template <typename... Args>
  void emplaceBack(Args&&... args);

  void popBack();

  T& operator[](size_t i) {
    return _data[i];
  }

  const T& operator[](size_t i) const {
    return _data[i];
  }

  T& at(size_t i);
  const T& at(size_t i) const;

  T* begin() {
    return _data;
  }

  T* end() {
    return _data + _size;
  }

  const T* begin() const {
    return _data;
  }

  const T* end() const {
    return _data + _size;
  }
};

extern template class DynArray<int>;
extern template class DynArray<String>;
extern template class DynArray<const char*>;

}  // namespace bmin

#include "internal/DynArray.hpp"  // IWYU pragma: keep
