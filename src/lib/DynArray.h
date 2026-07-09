#pragma once

#include "String.h"

#include "./internal/Types.h" // IWYU pragma: keep

#include <initializer_list>

namespace bmin {

template <typename T>
class DynArrayReverseIterator;

template <typename T>
class DynArray {
  T* _data = nullptr;
  size_t _size = 0;
  size_t _capacity = 0;

  void reallocate(size_t newCap);

public:
  DynArray();
  explicit DynArray(size_t count);
  DynArray(std::initializer_list<T> init);
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
  void resize(size_t n);
  void resize(size_t n, const T& value);

  void pushBack(const T& value);
  void pushBack(T&& value);

  template <typename... Args>
  void emplaceBack(Args&&... args);

  void popBack();

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

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

  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = DynArrayReverseIterator<T>;
  using ConstReverseIterator = DynArrayReverseIterator<const T>;

  Iterator insert(Iterator pos, const T& value);
  Iterator insert(Iterator pos, T&& value);

  Iterator erase(Iterator pos);
  Iterator erase(Iterator first, Iterator last);
  void erase(size_t index);

  template <typename Pred>
  size_t eraseIf(Pred pred);

  ReverseIterator rbegin() {
    return ReverseIterator(end());
  }

  ReverseIterator rend() {
    return ReverseIterator(begin());
  }

  ConstReverseIterator rbegin() const {
    return ConstReverseIterator(end());
  }

  ConstReverseIterator rend() const {
    return ConstReverseIterator(begin());
  }
};

template <typename T>
class DynArrayReverseIterator {
  T* _ptr = nullptr;

public:
  DynArrayReverseIterator() = default;
  explicit DynArrayReverseIterator(T* ptr) : _ptr(ptr) {}

  T& operator*() const {
    return *(_ptr - 1);
  }

  DynArrayReverseIterator& operator++() {
    --_ptr;
    return *this;
  }

  bool operator==(DynArrayReverseIterator o) const {
    return _ptr == o._ptr;
  }

  bool operator!=(DynArrayReverseIterator o) const {
    return !(*this == o);
  }
};

extern template class DynArray<int>;
extern template class DynArray<String>;
extern template class DynArray<const char*>;

}  // namespace bmin

#include "./internal/DynArray.hpp"  // IWYU pragma: keep
