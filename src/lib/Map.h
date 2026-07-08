#pragma once

#include "DynArray.h"
#include "Hash.h"
#include "List.h"
#include "String.h"

namespace bmin {

template <typename K, typename V, typename H = Hash<K>, typename E = EqualTo<K>>
class Map {
public:
  struct Entry {
    K key;
    V value;
  };

  class Iterator;

private:
  using BucketList = List<Entry>;

  DynArray<BucketList> _buckets;
  size_t _size = 0;
  H _hasher{};
  E _equal{};

  size_t bucketIndex(const K& key) const;
  Iterator findIterator(const K& key);
  void rehash(size_t newCap);

public:
  Map();
  Map(const Map& o);
  Map(Map&& o) noexcept;
  Map& operator=(Map o);
  ~Map() = default;

  size_t size() const {
    return _size;
  }

  bool empty() const {
    return _size == 0;
  }

  Iterator begin();
  Iterator end() const;

  Iterator find(const K& key);
  bool contains(const K& key) const;

  bool insert(K key, V value);
  V& operator[](const K& key);

  Iterator erase(Iterator it);
  bool erase(const K& key);
};

template <typename K, typename V, typename H, typename E>
class Map<K, V, H, E>::Iterator {
  Map* _map = nullptr;
  size_t _bucket = 0;
  typename BucketList::Iterator _inner{};

  void advancePastEmpty();

  Iterator(Map* m, size_t b, typename BucketList::Iterator it)
      : _map(m), _bucket(b), _inner(it) {}

  friend class Map;

public:
  Entry& operator*() const {
    return *_inner;
  }

  Entry* operator->() const {
    return &*_inner;
  }

  Iterator& operator++() {
    ++_inner;
    advancePastEmpty();
    return *this;
  }

  bool operator==(const Iterator& o) const {
    return _map == o._map && _bucket == o._bucket && _inner == o._inner;
  }

  bool operator!=(const Iterator& o) const {
    return !(*this == o);
  }
};

extern template class Map<String, int>;
extern template class Map<String, String>;

}  // namespace bmin

#include "internal/Map.hpp"  // IWYU pragma: keep

/*
 * Iterator invalidation:
 * - erase(it) invalidates only it
 * - insert / operator[] may rehash and invalidate all iterators
 */
