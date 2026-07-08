#pragma once

#include "String.h"

#include "internal/Types.h"  // IWYU pragma: keep

namespace bmin {

template <typename T>
class List {
  struct Node;

  Node* head_ = nullptr;
  Node* tail_ = nullptr;
  size_t size_ = 0;

  void unlink(Node* node);

public:
  class Iterator;

  List();
  List(const List& o);
  List(List&& o) noexcept;
  List& operator=(List o);
  ~List();

  size_t size() const {
    return size_;
  }

  bool empty() const {
    return size_ == 0;
  }

  Iterator begin() const;
  Iterator end() const;

  void clear();

  void push_back(const T& value);
  void push_back(T&& value);
  void push_front(const T& value);
  void push_front(T&& value);

  void pop_back();
  void pop_front();

  Iterator erase(Iterator it);
};

template <typename T>
class List<T>::Iterator {
  Node* node_ = nullptr;

  explicit Iterator(Node* n);

  friend class List;

public:
  Iterator() = default;

  T& operator*() const;
  Iterator& operator++();

  bool operator==(Iterator o) const;
  bool operator!=(Iterator o) const;
};

extern template class List<int>;
extern template class List<String>;

}  // namespace bmin

#include "List.hpp"  // IWYU pragma: keep
