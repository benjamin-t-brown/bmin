#pragma once

#include "Utility.h"

#include "Types.h"

namespace bmin {
namespace internal {

template <typename T>
class List {
  struct Node {
    T value;
    Node* prev;
    Node* next;
  };

  Node* head_ = nullptr;
  Node* tail_ = nullptr;
  size_t size_ = 0;

  void unlink(Node* node) {
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

public:
  class Iterator {
    Node* node_ = nullptr;

    explicit Iterator(Node* n) : node_(n) {}

    friend class List;

  public:
    Iterator() = default;
    T& operator*() const {
      return node_->value;
    }

    Iterator& operator++() {
      node_ = node_->next;
      return *this;
    }

    bool operator==(Iterator o) const {
      return node_ == o.node_;
    }

    bool operator!=(Iterator o) const {
      return !(*this == o);
    }
  };

  List() = default;

  List(const List& o) : List() {
    for (const T& v : o)
      push_back(v);
  }

  List(List&& o) noexcept
      : head_(o.head_), tail_(o.tail_), size_(o.size_) {
    o.head_ = nullptr;
    o.tail_ = nullptr;
    o.size_ = 0;
  }

  List& operator=(List o) {
    swap(head_, o.head_);
    swap(tail_, o.tail_);
    swap(size_, o.size_);
    return *this;
  }

  ~List() {
    clear();
  }

  size_t size() const {
    return size_;
  }

  bool empty() const {
    return size_ == 0;
  }

  Iterator begin() const {
    return Iterator(head_);
  }

  Iterator end() const {
    return Iterator(nullptr);
  }

  void clear() {
    while (head_)
      unlink(head_);
  }

  void push_back(const T& value) {
    Node* n = new Node{value, tail_, nullptr};
    if (tail_)
      tail_->next = n;
    else
      head_ = n;
    tail_ = n;
    ++size_;
  }

  void push_back(T&& value) {
    Node* n = new Node{bmin::move(value), tail_, nullptr};
    if (tail_)
      tail_->next = n;
    else
      head_ = n;
    tail_ = n;
    ++size_;
  }

  Iterator erase(Iterator it) {
    Node* next = it.node_ ? it.node_->next : nullptr;
    if (it.node_)
      unlink(it.node_);
    return Iterator(next);
  }
};

}  // namespace internal
}  // namespace bmin
