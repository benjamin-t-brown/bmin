module;
#include <cstddef>
#include "assert.h"

export module bmin.queue;
export import bmin.dynarray;

export namespace bmin {

template <typename T>
class Queue {
  DynArray<T> _buf;
  std::size_t _head = 0;
  std::size_t _tail = 0;
  std::size_t _count = 0;

  void grow();

public:
  Queue() = default;

  void push(const T& value);
  void push(T&& value);

  template <typename... Args>
  void emplace(Args&&... args);

  void pop();
  T& front();
  const T& front() const;

  bool empty() const {
    return _count == 0;
  }

  std::size_t size() const {
    return _count;
  }

  void clear();
};

template <typename T>
void Queue<T>::grow() {
  std::size_t cap = _buf.capacity();
  std::size_t newCap = cap ? cap * 2 : 4;
  DynArray<T> fresh;
  fresh.reserve(newCap);
  for (std::size_t i = 0; i < _count; ++i) {
    std::size_t idx = cap ? (_head + i) % cap : 0;
    fresh.pushBack(bmin::move(_buf[idx]));
    detail::storage::destroyAt(&_buf[idx]);
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
  std::size_t cap = _buf.capacity();
  detail::storage::constructAt(&_buf[_tail], value);
  _tail = (_tail + 1) % cap;
  ++_count;
}

template <typename T>
void Queue<T>::push(T&& value) {
  if (_count == _buf.capacity()) {
    grow();
  }
  std::size_t cap = _buf.capacity();
  detail::storage::constructAt(&_buf[_tail], bmin::move(value));
  _tail = (_tail + 1) % cap;
  ++_count;
}

template <typename T>
template <typename... Args>
void Queue<T>::emplace(Args&&... args) {
  if (_count == _buf.capacity()) {
    grow();
  }
  std::size_t cap = _buf.capacity();
  detail::storage::constructAt(&_buf[_tail], bmin::forward<Args>(args)...);
  _tail = (_tail + 1) % cap;
  ++_count;
}

template <typename T>
void Queue<T>::pop() {
  BMIN_ASSERT(_count > 0);
  std::size_t cap = _buf.capacity();
  detail::storage::destroyAt(&_buf[_head]);
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
