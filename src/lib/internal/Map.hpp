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
void Map<K, V, H, E>::ConstIterator::advancePastEmpty() {
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
template <typename Q>
size_t Map<K, V, H, E>::bucketIndex(const Q& key) const {
  return _buckets.size() ? _hasher(key) % _buckets.size() : 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map() : _buckets(16) {}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(H hasher, E equal)
    : _buckets(16), _hasher(bmin::move(hasher)), _equal(bmin::move(equal)) {}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(const Map& o)
    : _buckets(16), _size(0), _hasher(o._hasher), _equal(o._equal) {
  reserve(o._size);
  for (size_t b = 0; b < o._buckets.size(); ++b) {
    for (typename BucketList::ConstIterator it = o._buckets[b].begin();
         it != o._buckets[b].end(); ++it) {
      insert((*it).key, (*it).value);
    }
  }
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(Map&& o) noexcept
    : _buckets(bmin::move(o._buckets)),
      _size(o._size),
      _hasher(bmin::move(o._hasher)),
      _equal(bmin::move(o._equal)) {
  o._size = 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>& Map<K, V, H, E>::operator=(Map o) {
  detail::swap(_buckets, o._buckets);
  detail::swap(_size, o._size);
  detail::swap(_hasher, o._hasher);
  detail::swap(_equal, o._equal);
  return *this;
}

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::rehash(size_t newCap) {
  if (newCap < 16) {
    newCap = 16;
  }
  DynArray<BucketList> newBuckets(newCap);
  for (size_t b = 0; b < _buckets.size(); ++b) {
    typename BucketList::Iterator it = _buckets[b].begin();
    while (it != _buckets[b].end()) {
      typename BucketList::Iterator next = it;
      ++next;
      const size_t h = _hasher((*it).key) % newCap;
      newBuckets[h].splice(newBuckets[h].end(), _buckets[b], it);
      it = next;
    }
  }
  _buckets = bmin::move(newBuckets);
}

template <typename K, typename V, typename H, typename E>
template <typename Q>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::findIterator(
    const Q& key) {
  if (_buckets.empty()) {
    return end();
  }
  const size_t b = bucketIndex(key);
  for (typename BucketList::Iterator it = _buckets[b].begin();
       it != _buckets[b].end(); ++it) {
    if (_equal((*it).key, key)) {
      return Iterator(this, b, it);
    }
  }
  return end();
}

template <typename K, typename V, typename H, typename E>
template <typename Q>
typename Map<K, V, H, E>::ConstIterator Map<K, V, H, E>::findIterator(
    const Q& key) const {
  if (_buckets.empty()) {
    return end();
  }
  const size_t b = bucketIndex(key);
  for (typename BucketList::ConstIterator it = _buckets[b].begin();
       it != _buckets[b].end(); ++it) {
    if (_equal((*it).key, key)) {
      return ConstIterator(this, b, it);
    }
  }
  return end();
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::begin() {
  if (_buckets.empty()) {
    return end();
  }
  Iterator it(this, 0, _buckets[0].begin());
  it.advancePastEmpty();
  return it;
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::end() {
  return Iterator(this, _buckets.size(), typename BucketList::Iterator());
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::ConstIterator Map<K, V, H, E>::begin() const {
  if (_buckets.empty()) {
    return end();
  }
  ConstIterator it(this, 0, _buckets[0].begin());
  it.advancePastEmpty();
  return it;
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::ConstIterator Map<K, V, H, E>::end() const {
  return ConstIterator(
      this, _buckets.size(), typename BucketList::ConstIterator());
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::find(const K& key) {
  return findIterator(key);
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::ConstIterator Map<K, V, H, E>::find(
    const K& key) const {
  return findIterator(key);
}

template <typename K, typename V, typename H, typename E>
template <typename Q>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::find(const Q& key) {
  return findIterator(key);
}

template <typename K, typename V, typename H, typename E>
template <typename Q>
typename Map<K, V, H, E>::ConstIterator Map<K, V, H, E>::find(
    const Q& key) const {
  return findIterator(key);
}

template <typename K, typename V, typename H, typename E>
bool Map<K, V, H, E>::contains(const K& key) const {
  return findIterator(key) != end();
}

template <typename K, typename V, typename H, typename E>
template <typename Q>
bool Map<K, V, H, E>::contains(const Q& key) const {
  return findIterator(key) != end();
}

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::clear() {
  for (size_t b = 0; b < _buckets.size(); ++b) {
    _buckets[b].clear();
  }
  _size = 0;
}

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::reserve(size_t expectedEntries) {
  const size_t maxSize = static_cast<size_t>(-1);
  const size_t extraBuckets =
      expectedEntries / 3 + (expectedEntries % 3 != 0 ? 1 : 0);
  if (expectedEntries > maxSize - extraBuckets) {
    fatal();
  }
  const size_t requiredBuckets = expectedEntries + extraBuckets;
  if (requiredBuckets > _buckets.size()) {
    rehash(requiredBuckets);
  }
}

template <typename K, typename V, typename H, typename E>
bool Map<K, V, H, E>::insert(K key, V value) {
  if (_buckets.empty()) {
    _buckets = DynArray<BucketList>(16);
  }

  Iterator it = findIterator(key);
  if (it != end()) {
    (*it).value = bmin::move(value);
    return false;
  }

  const size_t loadLimit = _buckets.size() / 2 + _buckets.size() / 4;
  if (_size >= loadLimit) {
    if (_buckets.size() > static_cast<size_t>(-1) / 2) {
      fatal();
    }
    rehash(_buckets.size() * 2);
  }

  const size_t b = bucketIndex(key);
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
  const size_t b = it._bucket;
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

template <typename K, typename V, typename H, typename E>
template <typename Q>
bool Map<K, V, H, E>::erase(const Q& key) {
  Iterator it = findIterator(key);
  if (it == end()) {
    return false;
  }
  erase(it);
  return true;
}

}  // namespace bmin
