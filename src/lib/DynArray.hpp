#pragma once

#include "DynArray.h"

#include "internal/Storage.h"

namespace bmin {

template <typename T>
void DynArray<T>::reallocate(size_t newCap) {
  if (newCap == 0) {
    for (size_t i = 0; i < _size; ++i) {
      storage::destroyAt(_data + i);
    }
    storage::deallocateRaw(_data);
    _data = nullptr;
    _capacity = 0;
    _size = 0;
    return;
  }

  T* newData = static_cast<T*>(storage::allocateRaw(newCap * sizeof(T)));
  for (size_t i = 0; i < _size; ++i) {
    storage::constructAt(newData + i, bmin::move(_data[i]));
    storage::destroyAt(_data + i);
  }
  storage::deallocateRaw(_data);
  _data = newData;
  _capacity = newCap;
}

template <typename T>
DynArray<T>::DynArray() = default;

template <typename T>
DynArray<T>::DynArray(size_t count) : _size(count), _capacity(count) {
  if (count == 0) {
    return;
  }
  _data = static_cast<T*>(storage::allocateRaw(count * sizeof(T)));
  for (size_t i = 0; i < count; ++i) {
    storage::constructAt(_data + i);
  }
}

template <typename T>
DynArray<T>::DynArray(const DynArray& o) {
  reserve(o._size);
  for (size_t i = 0; i < o._size; ++i) {
    pushBack(o._data[i]);
  }
}

template <typename T>
DynArray<T>::DynArray(DynArray&& o) noexcept
    : _data(o._data), _size(o._size), _capacity(o._capacity) {
  o._data = nullptr;
  o._size = 0;
  o._capacity = 0;
}

template <typename T>
DynArray<T>::~DynArray() {
  reallocate(0);
}

template <typename T>
DynArray<T>& DynArray<T>::operator=(DynArray o) {
  bmin::detail::swap(_data, o._data);
  bmin::detail::swap(_size, o._size);
  bmin::detail::swap(_capacity, o._capacity);
  return *this;
}

template <typename T>
void DynArray<T>::reserve(size_t n) {
  if (n > _capacity) {
    reallocate(n);
  }
}

template <typename T>
void DynArray<T>::clear() {
  for (size_t i = 0; i < _size; ++i) {
    storage::destroyAt(_data + i);
  }
  _size = 0;
}

template <typename T>
void DynArray<T>::pushBack(const T& value) {
  emplaceBack(value);
}

template <typename T>
void DynArray<T>::pushBack(T&& value) {
  emplaceBack(bmin::move(value));
}

template <typename T>
template <typename... Args>
void DynArray<T>::emplaceBack(Args&&... args) {
  if (_size == _capacity) {
    size_t newCap = _capacity ? _capacity * 2 : 1;
    reallocate(newCap);
  }
  storage::constructAt(_data + _size, bmin::forward<Args>(args)...);
  ++_size;
}

template <typename T>
void DynArray<T>::popBack() {
  BMIN_ASSERT(_size > 0);
  --_size;
  storage::destroyAt(_data + _size);
}

template <typename T>
T& DynArray<T>::at(size_t i) {
  BMIN_ASSERT(i < _size);
  return _data[i];
}

template <typename T>
const T& DynArray<T>::at(size_t i) const {
  BMIN_ASSERT(i < _size);
  return _data[i];
}

}  // namespace bmin
