#pragma once

#include "../Map.h"

namespace bmin {

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::Iterator::advancePastEmpty() {
  if (!_map) {
    return;
  }
  while (_bucket < _map->_buckets.size() &&
         _inner == _map->_buckets[_bucket].end()) {
    ++_bucket;
    if (_bucket < _map->_buckets.size()) {
      _inner = _map->_buckets[_bucket].begin();
    }
  }
}

template <typename K, typename V, typename H, typename E>
size_t Map<K, V, H, E>::bucketIndex(const K& key) const {
  return _buckets.size() ? _hasher(key) % _buckets.size() : 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map() : _buckets(16) {}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(const Map& o) : _buckets(16), _size(0) {
  for (size_t b = 0; b < o._buckets.size(); ++b) {
    for (typename BucketList::Iterator it = o._buckets[b].begin();
         it != o._buckets[b].end(); ++it) {
      insert((*it).key, (*it).value);
    }
  }
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(Map&& o) noexcept
    : _buckets(bmin::move(o._buckets)), _size(o._size) {
  o._size = 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>& Map<K, V, H, E>::operator=(Map o) {
  detail::swap(_buckets, o._buckets);
  detail::swap(_size, o._size);
  return *this;
}

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::rehash(size_t newCap) {
  if (newCap < 16) {
    newCap = 16;
  }
  DynArray<BucketList> newBuckets(newCap);
  for (size_t b = 0; b < _buckets.size(); ++b) {
    while (!_buckets[b].empty()) {
      typename BucketList::Iterator it = _buckets[b].begin();
      Entry e = bmin::move(*it);
      _buckets[b].erase(it);
      size_t h = _hasher(e.key) % newCap;
      newBuckets[h].pushBack(bmin::move(e));
    }
  }
  _buckets = bmin::move(newBuckets);
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::findIterator(
    const K& key) {
  if (_buckets.empty()) {
    return end();
  }
  size_t b = bucketIndex(key);
  for (typename BucketList::Iterator it = _buckets[b].begin();
       it != _buckets[b].end(); ++it) {
    if (_equal((*it).key, key)) {
      return Iterator(this, b, it);
    }
  }
  return end();
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::begin() {
  if (_buckets.size() == 0) {
    return end();
  }
  Iterator it(this, 0, _buckets[0].begin());
  it.advancePastEmpty();
  return it;
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::end() const {
  return Iterator(const_cast<Map*>(this), _buckets.size(),
                  typename BucketList::Iterator());
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::find(const K& key) {
  return findIterator(key);
}

template <typename K, typename V, typename H, typename E>
bool Map<K, V, H, E>::contains(const K& key) const {
  return const_cast<Map*>(this)->findIterator(key) != end();
}

template <typename K, typename V, typename H, typename E>
bool Map<K, V, H, E>::insert(K key, V value) {
  if (_buckets.size() == 0) {
    _buckets = DynArray<BucketList>(16);
  }
  if (_size >= _buckets.size() * 3 / 4) {
    rehash(_buckets.size() * 2);
  }

  Iterator it = findIterator(key);
  if (it != end()) {
    (*it).value = bmin::move(value);
    return false;
  }

  size_t b = bucketIndex(key);
  _buckets[b].pushBack(Entry{bmin::move(key), bmin::move(value)});
  ++_size;
  return true;
}

template <typename K, typename V, typename H, typename E>
V& Map<K, V, H, E>::operator[](const K& key) {
  Iterator it = findIterator(key);
  if (it != end()) {
    return (*it).value;
  }
  insert(key, V{});
  return (*findIterator(key)).value;
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::erase(Iterator it) {
  if (it == end()) {
    return it;
  }
  size_t b = it._bucket;
  typename BucketList::Iterator next = it._inner;
  ++next;
  _buckets[b].erase(it._inner);
  --_size;
  Iterator out(this, b, next);
  out.advancePastEmpty();
  return out;
}

template <typename K, typename V, typename H, typename E>
bool Map<K, V, H, E>::erase(const K& key) {
  Iterator it = findIterator(key);
  if (it == end()) {
    return false;
  }
  erase(it);
  return true;
}

}  // namespace bmin
