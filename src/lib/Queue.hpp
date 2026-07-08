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
  size_t cap = buf_.capacity();
  size_t newCap = cap ? cap * 2 : 4;
  DynArray<T> fresh;
  fresh.reserve(newCap);
  for (size_t i = 0; i < count_; ++i) {
    size_t idx = cap ? (head_ + i) % cap : 0;
    fresh.push_back(bmin::move(buf_[idx]));
    storage::destroyAt(&buf_[idx]);
  }
  buf_ = bmin::move(fresh);
  head_ = 0;
  tail_ = count_;
}

template <typename T>
void Queue<T>::push(const T& value) {
  if (count_ == buf_.capacity())
    grow();
  size_t cap = buf_.capacity();
  storage::constructAt(&buf_[tail_], value);
  tail_ = (tail_ + 1) % cap;
  ++count_;
}

template <typename T>
void Queue<T>::push(T&& value) {
  if (count_ == buf_.capacity())
    grow();
  size_t cap = buf_.capacity();
  storage::constructAt(&buf_[tail_], bmin::move(value));
  tail_ = (tail_ + 1) % cap;
  ++count_;
}

template <typename T>
template <typename... Args>
void Queue<T>::emplace(Args&&... args) {
  if (count_ == buf_.capacity())
    grow();
  size_t cap = buf_.capacity();
  storage::constructAt(&buf_[tail_], bmin::forward<Args>(args)...);
  tail_ = (tail_ + 1) % cap;
  ++count_;
}

template <typename T>
void Queue<T>::pop() {
  BMIN_ASSERT(count_ > 0);
  size_t cap = buf_.capacity();
  storage::destroyAt(&buf_[head_]);
  head_ = (head_ + 1) % cap;
  --count_;
}

template <typename T>
T& Queue<T>::front() {
  BMIN_ASSERT(count_ > 0);
  return buf_[head_];
}

template <typename T>
const T& Queue<T>::front() const {
  BMIN_ASSERT(count_ > 0);
  return buf_[head_];
}

template <typename T>
void Queue<T>::clear() {
  while (count_ > 0)
    pop();
  head_ = 0;
  tail_ = 0;
}

}  // namespace bmin
