module;
#include <cstddef>
#include <initializer_list>
#include "assert.h"

export module bmin.list;
export import bmin.core;

export namespace bmin {

template <typename T>
class List {
  struct Node;

  Node* _head = nullptr;
  Node* _tail = nullptr;
  std::size_t _size = 0;

  void unlink(Node* node);

public:
  class Iterator;
  class ConstIterator;

  List();
  List(std::initializer_list<T> init);
  List(const List& o);
  List(List&& o) noexcept;
  List& operator=(List o);
  ~List();

  std::size_t size() const {
    return _size;
  }

  bool empty() const {
    return _size == 0;
  }

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;

  void clear();

  void pushBack(const T& value);
  void pushBack(T&& value);
  void pushFront(const T& value);
  void pushFront(T&& value);

  void popBack();
  void popFront();

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  void splice(Iterator pos, List& other);
  void splice(Iterator pos, List& other, Iterator it);

  Iterator erase(Iterator it);

  bool contains(const T& value) const;
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

template <typename T>
class List<T>::ConstIterator {
  const Node* _node = nullptr;

  explicit ConstIterator(const Node* n);

  friend class List;

public:
  ConstIterator() = default;

  const T& operator*() const;
  ConstIterator& operator++();

  bool operator==(ConstIterator o) const;
  bool operator!=(ConstIterator o) const;
};

template <typename T>
struct List<T>::Node {
  T value;
  Node* prev;
  Node* next;
};

template <typename T>
void List<T>::unlink(Node* node) {
  if (node->prev) {
    node->prev->next = node->next;
  } else {
    _head = node->next;
  }
  if (node->next) {
    node->next->prev = node->prev;
  } else {
    _tail = node->prev;
  }
  delete node;
  --_size;
}

template <typename T>
List<T>::List() = default;

template <typename T>
List<T>::List(std::initializer_list<T> init) : List() {
  for (const T& value : init) {
    pushBack(value);
  }
}

template <typename T>
List<T>::List(const List& o) : List() {
  for (const T& v : o) {
    pushBack(v);
  }
}

template <typename T>
List<T>::List(List&& o) noexcept
    : _head(o._head), _tail(o._tail), _size(o._size) {
  o._head = nullptr;
  o._tail = nullptr;
  o._size = 0;
}

template <typename T>
List<T>::~List() {
  clear();
}

template <typename T>
List<T>& List<T>::operator=(List o) {
  bmin::detail::swap(_head, o._head);
  bmin::detail::swap(_tail, o._tail);
  bmin::detail::swap(_size, o._size);
  return *this;
}

template <typename T>
typename List<T>::Iterator List<T>::begin() {
  return Iterator(_head);
}

template <typename T>
typename List<T>::Iterator List<T>::end() {
  return Iterator(nullptr);
}

template <typename T>
typename List<T>::ConstIterator List<T>::begin() const {
  return ConstIterator(_head);
}

template <typename T>
typename List<T>::ConstIterator List<T>::end() const {
  return ConstIterator(nullptr);
}

template <typename T>
void List<T>::clear() {
  while (_head) {
    unlink(_head);
  }
}

template <typename T>
void List<T>::pushBack(const T& value) {
  Node* n = new Node{value, _tail, nullptr};
  if (_tail) {
    _tail->next = n;
  } else {
    _head = n;
  }
  _tail = n;
  ++_size;
}

template <typename T>
void List<T>::pushBack(T&& value) {
  Node* n = new Node{bmin::move(value), _tail, nullptr};
  if (_tail) {
    _tail->next = n;
  } else {
    _head = n;
  }
  _tail = n;
  ++_size;
}

template <typename T>
void List<T>::pushFront(const T& value) {
  Node* n = new Node{value, nullptr, _head};
  if (_head) {
    _head->prev = n;
  } else {
    _tail = n;
  }
  _head = n;
  ++_size;
}

template <typename T>
void List<T>::pushFront(T&& value) {
  Node* n = new Node{bmin::move(value), nullptr, _head};
  if (_head) {
    _head->prev = n;
  } else {
    _tail = n;
  }
  _head = n;
  ++_size;
}

template <typename T>
void List<T>::popBack() {
  BMIN_ASSERT(_tail);
  unlink(_tail);
}

template <typename T>
void List<T>::popFront() {
  BMIN_ASSERT(_head);
  unlink(_head);
}

template <typename T>
T& List<T>::front() {
  BMIN_ASSERT(_head);
  return _head->value;
}

template <typename T>
const T& List<T>::front() const {
  BMIN_ASSERT(_head);
  return _head->value;
}

template <typename T>
T& List<T>::back() {
  BMIN_ASSERT(_tail);
  return _tail->value;
}

template <typename T>
const T& List<T>::back() const {
  BMIN_ASSERT(_tail);
  return _tail->value;
}

template <typename T>
void List<T>::splice(Iterator pos, List& other) {
  if (other.empty()) {
    return;
  }

  Node* before = pos._node;
  Node* first = other._head;
  Node* last = other._tail;

  if (before == nullptr) {
    if (_tail) {
      _tail->next = first;
      first->prev = _tail;
    } else {
      _head = first;
    }
    _tail = last;
  } else {
    Node* prev = before->prev;
    if (prev) {
      prev->next = first;
      first->prev = prev;
    } else {
      _head = first;
    }
    last->next = before;
    before->prev = last;
  }

  _size += other._size;
  other._head = nullptr;
  other._tail = nullptr;
  other._size = 0;
}

template <typename T>
void List<T>::splice(Iterator pos, List& other, Iterator it) {
  Node* node = it._node;
  if (!node) {
    return;
  }
  if (this == &other && (pos._node == node || pos._node == node->next)) {
    return;
  }

  if (node->prev) {
    node->prev->next = node->next;
  } else {
    other._head = node->next;
  }
  if (node->next) {
    node->next->prev = node->prev;
  } else {
    other._tail = node->prev;
  }
  --other._size;

  Node* before = pos._node;
  if (!before) {
    node->prev = _tail;
    node->next = nullptr;
    if (_tail) {
      _tail->next = node;
    } else {
      _head = node;
    }
    _tail = node;
  } else {
    node->prev = before->prev;
    node->next = before;
    if (before->prev) {
      before->prev->next = node;
    } else {
      _head = node;
    }
    before->prev = node;
  }
  ++_size;
}

template <typename T>
typename List<T>::Iterator List<T>::erase(Iterator it) {
  Node* next = it._node ? it._node->next : nullptr;
  if (it._node) {
    unlink(it._node);
  }
  return Iterator(next);
}

template <typename T>
bool List<T>::contains(const T& value) const {
  for (ConstIterator it = begin(); it != end(); ++it) {
    if (*it == value) {
      return true;
    }
  }
  return false;
}

template <typename T>
List<T>::Iterator::Iterator(Node* n) : _node(n) {}

template <typename T>
T& List<T>::Iterator::operator*() const {
  return _node->value;
}

template <typename T>
typename List<T>::Iterator& List<T>::Iterator::operator++() {
  _node = _node->next;
  return *this;
}

template <typename T>
bool List<T>::Iterator::operator==(Iterator o) const {
  return _node == o._node;
}

template <typename T>
bool List<T>::Iterator::operator!=(Iterator o) const {
  return !(*this == o);
}

template <typename T>
List<T>::ConstIterator::ConstIterator(const Node* n) : _node(n) {}

template <typename T>
const T& List<T>::ConstIterator::operator*() const {
  return _node->value;
}

template <typename T>
typename List<T>::ConstIterator& List<T>::ConstIterator::operator++() {
  _node = _node->next;
  return *this;
}

template <typename T>
bool List<T>::ConstIterator::operator==(ConstIterator o) const {
  return _node == o._node;
}

template <typename T>
bool List<T>::ConstIterator::operator!=(ConstIterator o) const {
  return !(*this == o);
}

}  // namespace bmin
