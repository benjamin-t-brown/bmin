#pragma once

// Included by Queue.h at the end of the header. Re-includes Queue.h so this
// file can be parsed alone (clangd); #pragma once breaks the cycle.
#include "Queue.h"  // IWYU pragma: keep

#include "internal/Policy.h"
#include "internal/Storage.h"
#include "internal/Utility.h"

namespace bmin {

template <typename T>
void Queue<T>::grow() {
  size_t cap = _buf.capacity();
  size_t newCap = cap ? cap * 2 : 4;
  DynArray<T> fresh;
  fresh.reserve(newCap);
  for (size_t i = 0; i < _count; ++i) {
    size_t idx = cap ? (_head + i) % cap : 0;
    fresh.pushBack(bmin::move(_buf[idx]));
    storage::destroyAt(&_buf[idx]);
  }
  _buf = bmin::move(fresh);
  _head = 0;
  _tail = _count;
}

template <typename T>
void Queue<T>::push(const T& value) {
  if (_count == _buf.capacity()) {
    grow();
  }
  size_t cap = _buf.capacity();
  storage::constructAt(&_buf[_tail], value);
  _tail = (_tail + 1) % cap;
  ++_count;
}

template <typename T>
void Queue<T>::push(T&& value) {
  if (_count == _buf.capacity()) {
    grow();
  }
  size_t cap = _buf.capacity();
  storage::constructAt(&_buf[_tail], bmin::move(value));
  _tail = (_tail + 1) % cap;
  ++_count;
}

template <typename T>
template <typename... Args>
void Queue<T>::emplace(Args&&... args) {
  if (_count == _buf.capacity()) {
    grow();
  }
  size_t cap = _buf.capacity();
  storage::constructAt(&_buf[_tail], bmin::forward<Args>(args)...);
  _tail = (_tail + 1) % cap;
  ++_count;
}

template <typename T>
void Queue<T>::pop() {
  BMIN_ASSERT(_count > 0);
  size_t cap = _buf.capacity();
  storage::destroyAt(&_buf[_head]);
  _head = (_head + 1) % cap;
  --_count;
}

template <typename T>
T& Queue<T>::front() {
  BMIN_ASSERT(_count > 0);
  return _buf[_head];
}

template <typename T>
const T& Queue<T>::front() const {
  BMIN_ASSERT(_count > 0);
  return _buf[_head];
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
