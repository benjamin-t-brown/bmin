#pragma once

#include "DynArray.h"

#include "internal/Storage.h"

namespace bmin {

template <typename T>
void DynArray<T>::reallocate(size_t newCap) {
  if (newCap == 0) {
    for (size_t i = 0; i < size_; ++i)
      storage::destroyAt(data_ + i);
    storage::deallocateRaw(data_);
    data_ = nullptr;
    capacity_ = 0;
    size_ = 0;
    return;
  }

  T* newData = static_cast<T*>(storage::allocateRaw(newCap * sizeof(T)));
  for (size_t i = 0; i < size_; ++i) {
    storage::constructAt(newData + i, bmin::move(data_[i]));
    storage::destroyAt(data_ + i);
  }
  storage::deallocateRaw(data_);
  data_ = newData;
  capacity_ = newCap;
}

template <typename T>
DynArray<T>::DynArray() = default;

template <typename T>
DynArray<T>::DynArray(size_t count) : size_(count), capacity_(count) {
  if (count == 0)
    return;
  data_ = static_cast<T*>(storage::allocateRaw(count * sizeof(T)));
  for (size_t i = 0; i < count; ++i)
    storage::constructAt(data_ + i);
}

template <typename T>
DynArray<T>::DynArray(const DynArray& o) {
  reserve(o.size_);
  for (size_t i = 0; i < o.size_; ++i)
    push_back(o.data_[i]);
}

template <typename T>
DynArray<T>::DynArray(DynArray&& o) noexcept
    : data_(o.data_), size_(o.size_), capacity_(o.capacity_) {
  o.data_ = nullptr;
  o.size_ = 0;
  o.capacity_ = 0;
}

template <typename T>
DynArray<T>::~DynArray() {
  reallocate(0);
}

template <typename T>
DynArray<T>& DynArray<T>::operator=(DynArray o) {
  bmin::swap(data_, o.data_);
  bmin::swap(size_, o.size_);
  bmin::swap(capacity_, o.capacity_);
  return *this;
}

template <typename T>
void DynArray<T>::reserve(size_t n) {
  if (n > capacity_)
    reallocate(n);
}

template <typename T>
void DynArray<T>::clear() {
  for (size_t i = 0; i < size_; ++i)
    storage::destroyAt(data_ + i);
  size_ = 0;
}

template <typename T>
void DynArray<T>::push_back(const T& value) {
  emplace_back(value);
}

template <typename T>
void DynArray<T>::push_back(T&& value) {
  emplace_back(bmin::move(value));
}

template <typename T>
template <typename... Args>
void DynArray<T>::emplace_back(Args&&... args) {
  if (size_ == capacity_) {
    size_t newCap = capacity_ ? capacity_ * 2 : 1;
    reallocate(newCap);
  }
  storage::constructAt(data_ + size_, bmin::forward<Args>(args)...);
  ++size_;
}

template <typename T>
void DynArray<T>::pop_back() {
  BMIN_ASSERT(size_ > 0);
  --size_;
  storage::destroyAt(data_ + size_);
}

template <typename T>
T& DynArray<T>::at(size_t i) {
  BMIN_ASSERT(i < size_);
  return data_[i];
}

template <typename T>
const T& DynArray<T>::at(size_t i) const {
  BMIN_ASSERT(i < size_);
  return data_[i];
}

}  // namespace bmin
