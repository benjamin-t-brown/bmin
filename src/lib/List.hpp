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
  if (node->prev)
    node->prev->next = node->next;
  else
    head_ = node->next;
  if (node->next)
    node->next->prev = node->prev;
  else
    tail_ = node->prev;
  delete node;
  --size_;
}

template <typename T>
List<T>::List() = default;

template <typename T>
List<T>::List(const List& o) : List() {
  for (const T& v : o)
    push_back(v);
}

template <typename T>
List<T>::List(List&& o) noexcept
    : head_(o.head_), tail_(o.tail_), size_(o.size_) {
  o.head_ = nullptr;
  o.tail_ = nullptr;
  o.size_ = 0;
}

template <typename T>
List<T>::~List() {
  clear();
}

template <typename T>
List<T>& List<T>::operator=(List o) {
  bmin::swap(head_, o.head_);
  bmin::swap(tail_, o.tail_);
  bmin::swap(size_, o.size_);
  return *this;
}

template <typename T>
typename List<T>::Iterator List<T>::begin() const {
  return Iterator(head_);
}

template <typename T>
typename List<T>::Iterator List<T>::end() const {
  return Iterator(nullptr);
}

template <typename T>
void List<T>::clear() {
  while (head_)
    unlink(head_);
}

template <typename T>
void List<T>::push_back(const T& value) {
  Node* n = new Node{value, tail_, nullptr};
  if (tail_)
    tail_->next = n;
  else
    head_ = n;
  tail_ = n;
  ++size_;
}

template <typename T>
void List<T>::push_back(T&& value) {
  Node* n = new Node{bmin::move(value), tail_, nullptr};
  if (tail_)
    tail_->next = n;
  else
    head_ = n;
  tail_ = n;
  ++size_;
}

template <typename T>
void List<T>::push_front(const T& value) {
  Node* n = new Node{value, nullptr, head_};
  if (head_)
    head_->prev = n;
  else
    tail_ = n;
  head_ = n;
  ++size_;
}

template <typename T>
void List<T>::push_front(T&& value) {
  Node* n = new Node{bmin::move(value), nullptr, head_};
  if (head_)
    head_->prev = n;
  else
    tail_ = n;
  head_ = n;
  ++size_;
}

template <typename T>
void List<T>::pop_back() {
  BMIN_ASSERT(tail_);
  unlink(tail_);
}

template <typename T>
void List<T>::pop_front() {
  BMIN_ASSERT(head_);
  unlink(head_);
}

template <typename T>
typename List<T>::Iterator List<T>::erase(Iterator it) {
  Node* next = it.node_ ? it.node_->next : nullptr;
  if (it.node_)
    unlink(it.node_);
  return Iterator(next);
}

template <typename T>
List<T>::Iterator::Iterator(Node* n) : node_(n) {}

template <typename T>
T& List<T>::Iterator::operator*() const {
  return node_->value;
}

template <typename T>
typename List<T>::Iterator& List<T>::Iterator::operator++() {
  node_ = node_->next;
  return *this;
}

template <typename T>
bool List<T>::Iterator::operator==(Iterator o) const {
  return node_ == o.node_;
}

template <typename T>
bool List<T>::Iterator::operator!=(Iterator o) const {
  return !(*this == o);
}

}  // namespace bmin
