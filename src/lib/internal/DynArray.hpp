#pragma once

#include "../DynArray.h"

#include "./Storage.h"

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
DynArray<T>::DynArray(std::initializer_list<T> init) {
  reserve(init.size());
  for (const T& value : init) {
    pushBack(value);
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
void DynArray<T>::resize(size_t n) {
  if (n > _size) {
    reserve(n);
    for (size_t i = _size; i < n; ++i) {
      storage::constructAt(_data + i);
    }
    _size = n;
  } else if (n < _size) {
    for (size_t i = n; i < _size; ++i) {
      storage::destroyAt(_data + i);
    }
    _size = n;
  }
}

template <typename T>
void DynArray<T>::resize(size_t n, const T& value) {
  const size_t oldSize = _size;
  if (n > _size) {
    reserve(n);
    for (size_t i = oldSize; i < n; ++i) {
      storage::constructAt(_data + i, value);
    }
    _size = n;
  } else if (n < _size) {
    for (size_t i = n; i < _size; ++i) {
      storage::destroyAt(_data + i);
    }
    _size = n;
  }
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::insert(Iterator pos, const T& value) {
  const size_t index = static_cast<size_t>(pos - _data);
  BMIN_ASSERT(index <= _size);
  if (_size == _capacity) {
    const size_t newCap = _capacity ? _capacity * 2 : 1;
    reallocate(newCap);
    pos = _data + index;
  }
  storage::constructAt(_data + _size);
  for (size_t i = _size; i > index; --i) {
    _data[i] = bmin::move(_data[i - 1]);
  }
  _data[index] = value;
  ++_size;
  return _data + index;
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::insert(Iterator pos, T&& value) {
  const size_t index = static_cast<size_t>(pos - _data);
  BMIN_ASSERT(index <= _size);
  if (_size == _capacity) {
    const size_t newCap = _capacity ? _capacity * 2 : 1;
    reallocate(newCap);
    pos = _data + index;
  }
  storage::constructAt(_data + _size);
  for (size_t i = _size; i > index; --i) {
    _data[i] = bmin::move(_data[i - 1]);
  }
  _data[index] = bmin::move(value);
  ++_size;
  return _data + index;
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::erase(Iterator pos) {
  const size_t index = static_cast<size_t>(pos - _data);
  BMIN_ASSERT(index < _size);
  for (size_t i = index; i + 1 < _size; ++i) {
    _data[i] = bmin::move(_data[i + 1]);
  }
  --_size;
  storage::destroyAt(_data + _size);
  return _data + index;
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::erase(Iterator first, Iterator last) {
  const size_t firstIndex = static_cast<size_t>(first - _data);
  const size_t lastIndex = static_cast<size_t>(last - _data);
  BMIN_ASSERT(firstIndex <= lastIndex && lastIndex <= _size);
  const size_t count = lastIndex - firstIndex;
  if (count == 0) {
    return first;
  }
  for (size_t i = firstIndex; i + count < _size; ++i) {
    _data[i] = bmin::move(_data[i + count]);
  }
  for (size_t i = _size - count; i < _size; ++i) {
    storage::destroyAt(_data + i);
  }
  _size -= count;
  return _data + firstIndex;
}

template <typename T>
void DynArray<T>::erase(size_t index) {
  erase(_data + index);
}

template <typename T>
template <typename Pred>
size_t DynArray<T>::eraseIf(Pred pred) {
  size_t write = 0;
  for (size_t read = 0; read < _size; ++read) {
    if (!pred(_data[read])) {
      if (write != read) {
        storage::destroyAt(_data + write);
        _data[write] = bmin::move(_data[read]);
      }
      ++write;
    }
  }
  for (size_t i = write; i < _size; ++i) {
    storage::destroyAt(_data + i);
  }
  const size_t removed = _size - write;
  _size = write;
  return removed;
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
T& DynArray<T>::front() {
  BMIN_ASSERT(_size > 0);
  return _data[0];
}

template <typename T>
const T& DynArray<T>::front() const {
  BMIN_ASSERT(_size > 0);
  return _data[0];
}

template <typename T>
T& DynArray<T>::back() {
  BMIN_ASSERT(_size > 0);
  return _data[_size - 1];
}

template <typename T>
const T& DynArray<T>::back() const {
  BMIN_ASSERT(_size > 0);
  return _data[_size - 1];
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
