#pragma once

#include "String.h"

#include "internal/Types.h"  // IWYU pragma: keep

namespace bmin {

template <typename T>
class List {
  struct Node;

  Node* _head = nullptr;
  Node* _tail = nullptr;
  size_t _size = 0;

  void unlink(Node* node);

public:
  class Iterator;

  List();
  List(const List& o);
  List(List&& o) noexcept;
  List& operator=(List o);
  ~List();

  size_t size() const {
    return _size;
  }

  bool empty() const {
    return _size == 0;
  }

  Iterator begin() const;
  Iterator end() const;

  void clear();

  void pushBack(const T& value);
  void pushBack(T&& value);
  void pushFront(const T& value);
  void pushFront(T&& value);

  void popBack();
  void popFront();

  Iterator erase(Iterator it);
};

template <typename T>
class List<T>::Iterator {
  Node* _node = nullptr;

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

#include "internal/List.hpp"  // IWYU pragma: keep
