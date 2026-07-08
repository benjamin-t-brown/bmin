#pragma once

#include "DynArray.h"

#include "internal/Types.h"  // IWYU pragma: keep

namespace bmin {

template <typename T>
class Queue {
  DynArray<T> buf_;
  size_t head_ = 0;
  size_t tail_ = 0;
  size_t count_ = 0;

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
    return count_ == 0;
  }

  size_t size() const {
    return count_;
  }

  void clear();
};

extern template class Queue<int>;
extern template class Queue<String>;

}  // namespace bmin

#include "Queue.hpp"  // IWYU pragma: keep
