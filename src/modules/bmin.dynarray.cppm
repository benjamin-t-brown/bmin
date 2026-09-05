module;
#include <cstddef>
#include <initializer_list>
#include "assert.h"

export module bmin.dynarray;
export import bmin.core;

export namespace bmin {

template <typename T>
class DynArrayReverseIterator;

template <typename T>
class DynArray {
  T* _data = nullptr;
  std::size_t _size = 0;
  std::size_t _capacity = 0;

  void reallocate(std::size_t newCap);

public:
  DynArray();
  explicit DynArray(std::size_t count);
  DynArray(std::initializer_list<T> init);
  DynArray(const DynArray& o);
  DynArray(DynArray&& o) noexcept;
  ~DynArray();

  DynArray& operator=(DynArray o);

  std::size_t size() const {
    return _size;
  }

  std::size_t capacity() const {
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

  void reserve(std::size_t n);
  void clear();
  void resize(std::size_t n);
  void resize(std::size_t n, const T& value);

  void pushBack(const T& value);
  void pushBack(T&& value);

  template <typename... Args>
  void emplaceBack(Args&&... args);

  void popBack();

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  T& operator[](std::size_t i) {
    return _data[i];
  }

  const T& operator[](std::size_t i) const {
    return _data[i];
  }

  T& at(std::size_t i);
  const T& at(std::size_t i) const;

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
  void erase(std::size_t index);

  template <typename Pred>
  std::size_t eraseIf(Pred pred);

  bool contains(const T& value) const;

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

template <typename T>
void DynArray<T>::reallocate(std::size_t newCap) {
  if (newCap == 0) {
    for (std::size_t i = 0; i < _size; ++i) {
      detail::storage::destroyAt(_data + i);
    }
    detail::storage::deallocate(_data);
    _data = nullptr;
    _capacity = 0;
    _size = 0;
    return;
  }

  T* newData = detail::storage::allocate<T>(newCap);
  for (std::size_t i = 0; i < _size; ++i) {
    detail::storage::constructAt(newData + i, bmin::move(_data[i]));
    detail::storage::destroyAt(_data + i);
  }
  detail::storage::deallocate(_data);
  _data = newData;
  _capacity = newCap;
}

template <typename T>
DynArray<T>::DynArray() = default;

template <typename T>
DynArray<T>::DynArray(std::size_t count) : _size(count), _capacity(count) {
  if (count == 0) {
    return;
  }
  _data = detail::storage::allocate<T>(count);
  for (std::size_t i = 0; i < count; ++i) {
    detail::storage::constructAt(_data + i);
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
  for (std::size_t i = 0; i < o._size; ++i) {
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
void DynArray<T>::reserve(std::size_t n) {
  if (n > _capacity) {
    reallocate(n);
  }
}

template <typename T>
void DynArray<T>::clear() {
  for (std::size_t i = 0; i < _size; ++i) {
    detail::storage::destroyAt(_data + i);
  }
  _size = 0;
}

template <typename T>
void DynArray<T>::resize(std::size_t n) {
  if (n > _size) {
    reserve(n);
    for (std::size_t i = _size; i < n; ++i) {
      detail::storage::constructAt(_data + i);
    }
    _size = n;
  } else if (n < _size) {
    for (std::size_t i = n; i < _size; ++i) {
      detail::storage::destroyAt(_data + i);
    }
    _size = n;
  }
}

template <typename T>
void DynArray<T>::resize(std::size_t n, const T& value) {
  const std::size_t oldSize = _size;
  if (n > _size) {
    reserve(n);
    for (std::size_t i = oldSize; i < n; ++i) {
      detail::storage::constructAt(_data + i, value);
    }
    _size = n;
  } else if (n < _size) {
    for (std::size_t i = n; i < _size; ++i) {
      detail::storage::destroyAt(_data + i);
    }
    _size = n;
  }
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::insert(Iterator pos, const T& value) {
  const std::size_t index = static_cast<std::size_t>(pos - _data);
  BMIN_ASSERT(index <= _size);
  if (_size == _capacity) {
    if (_capacity > static_cast<std::size_t>(-1) / 2) {
      detail::fatal();
    }
    const std::size_t newCap = _capacity ? _capacity * 2 : 1;
    reallocate(newCap);
    pos = _data + index;
  }
  detail::storage::constructAt(_data + _size);
  for (std::size_t i = _size; i > index; --i) {
    _data[i] = bmin::move(_data[i - 1]);
  }
  _data[index] = value;
  ++_size;
  return _data + index;
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::insert(Iterator pos, T&& value) {
  const std::size_t index = static_cast<std::size_t>(pos - _data);
  BMIN_ASSERT(index <= _size);
  if (_size == _capacity) {
    if (_capacity > static_cast<std::size_t>(-1) / 2) {
      detail::fatal();
    }
    const std::size_t newCap = _capacity ? _capacity * 2 : 1;
    reallocate(newCap);
    pos = _data + index;
  }
  detail::storage::constructAt(_data + _size);
  for (std::size_t i = _size; i > index; --i) {
    _data[i] = bmin::move(_data[i - 1]);
  }
  _data[index] = bmin::move(value);
  ++_size;
  return _data + index;
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::erase(Iterator pos) {
  const std::size_t index = static_cast<std::size_t>(pos - _data);
  BMIN_ASSERT(index < _size);
  for (std::size_t i = index; i + 1 < _size; ++i) {
    _data[i] = bmin::move(_data[i + 1]);
  }
  --_size;
  detail::storage::destroyAt(_data + _size);
  return _data + index;
}

template <typename T>
typename DynArray<T>::Iterator DynArray<T>::erase(Iterator first, Iterator last) {
  const std::size_t firstIndex = static_cast<std::size_t>(first - _data);
  const std::size_t lastIndex = static_cast<std::size_t>(last - _data);
  BMIN_ASSERT(firstIndex <= lastIndex && lastIndex <= _size);
  const std::size_t count = lastIndex - firstIndex;
  if (count == 0) {
    return first;
  }
  for (std::size_t i = firstIndex; i + count < _size; ++i) {
    _data[i] = bmin::move(_data[i + count]);
  }
  for (std::size_t i = _size - count; i < _size; ++i) {
    detail::storage::destroyAt(_data + i);
  }
  _size -= count;
  return _data + firstIndex;
}

template <typename T>
void DynArray<T>::erase(std::size_t index) {
  erase(_data + index);
}

template <typename T>
template <typename Pred>
std::size_t DynArray<T>::eraseIf(Pred pred) {
  std::size_t write = 0;
  for (std::size_t read = 0; read < _size; ++read) {
    if (!pred(_data[read])) {
      if (write != read) {
        _data[write] = bmin::move(_data[read]);
      }
      ++write;
    }
  }
  for (std::size_t i = write; i < _size; ++i) {
    detail::storage::destroyAt(_data + i);
  }
  const std::size_t removed = _size - write;
  _size = write;
  return removed;
}

template <typename T>
bool DynArray<T>::contains(const T& value) const {
  for (std::size_t i = 0; i < _size; ++i) {
    if (_data[i] == value) {
      return true;
    }
  }
  return false;
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
    if (_capacity > static_cast<std::size_t>(-1) / 2) {
      detail::fatal();
    }
    std::size_t newCap = _capacity ? _capacity * 2 : 1;
    reallocate(newCap);
  }
  detail::storage::constructAt(_data + _size,
                               bmin::forward<Args>(args)...);
  ++_size;
}

template <typename T>
void DynArray<T>::popBack() {
  BMIN_ASSERT(_size > 0);
  --_size;
  detail::storage::destroyAt(_data + _size);
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
T& DynArray<T>::at(std::size_t i) {
  if (i >= _size) {
    detail::fatal();
  }
  return _data[i];
}

template <typename T>
const T& DynArray<T>::at(std::size_t i) const {
  if (i >= _size) {
    detail::fatal();
  }
  return _data[i];
}

}  // namespace bmin
