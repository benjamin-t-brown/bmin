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

  DynArray<BucketList> buckets_;
  size_t size_ = 0;
  H hasher_{};
  E equal_{};

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
    return size_;
  }

  bool empty() const {
    return size_ == 0;
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
  Map* map_ = nullptr;
  size_t bucket_ = 0;
  typename BucketList::Iterator inner_{};

  void advancePastEmpty();

  Iterator(Map* m, size_t b, typename BucketList::Iterator it)
      : map_(m), bucket_(b), inner_(it) {}

  friend class Map;

public:
  Entry& operator*() const {
    return *inner_;
  }

  Entry* operator->() const {
    return &*inner_;
  }

  Iterator& operator++() {
    ++inner_;
    advancePastEmpty();
    return *this;
  }

  bool operator==(const Iterator& o) const {
    return map_ == o.map_ && bucket_ == o.bucket_ && inner_ == o.inner_;
  }

  bool operator!=(const Iterator& o) const {
    return !(*this == o);
  }
};

extern template class Map<String, int>;
extern template class Map<String, String>;

}  // namespace bmin

#include "Map.hpp"  // IWYU pragma: keep

/*
 * Iterator invalidation:
 * - erase(it) invalidates only it
 * - insert / operator[] may rehash and invalidate all iterators
 */
