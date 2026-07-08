#pragma once

#include "List.h"

#include "internal/Policy.h"
#include "internal/Utility.h"

namespace bmin {

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
  }
  else {
    _head = node->next;
  }
  if (node->next) {
    node->next->prev = node->prev;
  }
  else {
    _tail = node->prev;
  }
  delete node;
  --_size;
}

template <typename T>
List<T>::List() = default;

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
typename List<T>::Iterator List<T>::begin() const {
  return Iterator(_head);
}

template <typename T>
typename List<T>::Iterator List<T>::end() const {
  return Iterator(nullptr);
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
  }
  else {
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
  }
  else {
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
  }
  else {
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
  }
  else {
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
typename List<T>::Iterator List<T>::erase(Iterator it) {
  Node* next = it._node ? it._node->next : nullptr;
  if (it._node) {
    unlink(it._node);
  }
  return Iterator(next);
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

}  // namespace bmin
