#pragma once

// Included by Queue.h at the end of the header. Re-includes Queue.h so this
// file can be parsed alone (clangd); #pragma once breaks the cycle.
#include "../Queue.h"  // IWYU pragma: keep

#include "./Policy.h"
#include "./Storage.h"
#include "./Utility.h"

namespace bmin {

template <typename T>
void Queue<T>::grow() {
  if (_capacity > static_cast<size_t>(-1) / 2) {
    fatal();
  }
  const size_t newCap = _capacity ? _capacity * 2 : 4;
  T* fresh = storage::allocate<T>(newCap);
  for (size_t i = 0; i < _count; ++i) {
    const size_t idx = (_head + i) % _capacity;
    storage::constructAt(fresh + i, bmin::move(_data[idx]));
    storage::destroyAt(_data + idx);
  }
  storage::deallocate(_data);
  _data = fresh;
  _capacity = newCap;
  _head = 0;
  _tail = _count;
}

template <typename T>
Queue<T>::Queue(const Queue& o) {
  if (o._capacity) {
    _data = storage::allocate<T>(o._capacity);
    _capacity = o._capacity;
  }
  for (size_t i = 0; i < o._count; ++i) {
    const size_t idx = (o._head + i) % o._capacity;
    storage::constructAt(_data + i, o._data[idx]);
    ++_count;
  }
  _tail = _count == _capacity ? 0 : _count;
}

template <typename T>
Queue<T>::Queue(Queue&& o) noexcept
    : _data(o._data),
      _head(o._head),
      _tail(o._tail),
      _count(o._count),
      _capacity(o._capacity) {
  o._data = nullptr;
  o._head = 0;
  o._tail = 0;
  o._count = 0;
  o._capacity = 0;
}

template <typename T>
Queue<T>& Queue<T>::operator=(Queue o) {
  bmin::detail::swap(_data, o._data);
  bmin::detail::swap(_head, o._head);
  bmin::detail::swap(_tail, o._tail);
  bmin::detail::swap(_count, o._count);
  bmin::detail::swap(_capacity, o._capacity);
  return *this;
}

template <typename T>
Queue<T>::~Queue() {
  clear();
  storage::deallocate(_data);
}

template <typename T>
void Queue<T>::push(const T& value) {
  if (_count == _capacity) {
    grow();
  }
  storage::constructAt(_data + _tail, value);
  _tail = (_tail + 1) % _capacity;
  ++_count;
}

template <typename T>
void Queue<T>::push(T&& value) {
  if (_count == _capacity) {
    grow();
  }
  storage::constructAt(_data + _tail, bmin::move(value));
  _tail = (_tail + 1) % _capacity;
  ++_count;
}

template <typename T>
template <typename... Args>
void Queue<T>::emplace(Args&&... args) {
  if (_count == _capacity) {
    grow();
  }
  storage::constructAt(_data + _tail, bmin::forward<Args>(args)...);
  _tail = (_tail + 1) % _capacity;
  ++_count;
}

template <typename T>
void Queue<T>::pop() {
  BMIN_ASSERT(_count > 0);
  storage::destroyAt(_data + _head);
  _head = (_head + 1) % _capacity;
  --_count;
}

template <typename T>
T& Queue<T>::front() {
  BMIN_ASSERT(_count > 0);
  return _data[_head];
}

template <typename T>
const T& Queue<T>::front() const {
  BMIN_ASSERT(_count > 0);
  return _data[_head];
}

template <typename T>
T& Queue<T>::back() {
  BMIN_ASSERT(_count > 0);
  const size_t index = _tail == 0 ? _capacity - 1 : _tail - 1;
  return _data[index];
}

template <typename T>
const T& Queue<T>::back() const {
  BMIN_ASSERT(_count > 0);
  const size_t index = _tail == 0 ? _capacity - 1 : _tail - 1;
  return _data[index];
}

template <typename T>
void Queue<T>::clear() {
  while (_count > 0) {
    pop();
  }
  _head = 0;
  _tail = 0;
}

}  // namespace bmin
