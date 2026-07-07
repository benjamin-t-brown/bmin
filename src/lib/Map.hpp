#pragma once

#include "Map.h"

namespace bmin {

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::Iterator::advancePastEmpty() {
  if (!map_)
    return;
  while (bucket_ < map_->buckets_.size() &&
         inner_ == map_->buckets_[bucket_].end()) {
    ++bucket_;
    if (bucket_ < map_->buckets_.size())
      inner_ = map_->buckets_[bucket_].begin();
  }
}

template <typename K, typename V, typename H, typename E>
size_t Map<K, V, H, E>::bucketIndex(const K& key) const {
  return buckets_.size() ? hasher_(key) % buckets_.size() : 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map() : buckets_(16) {}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(const Map& o) : buckets_(16), size_(0) {
  for (size_t b = 0; b < o.buckets_.size(); ++b) {
    for (typename BucketList::Iterator it = o.buckets_[b].begin();
         it != o.buckets_[b].end(); ++it) {
      insert((*it).key, (*it).value);
    }
  }
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(Map&& o) noexcept
    : buckets_(bmin::move(o.buckets_)), size_(o.size_) {
  o.size_ = 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>& Map<K, V, H, E>::operator=(Map o) {
  swap(buckets_, o.buckets_);
  swap(size_, o.size_);
  return *this;
}

template <typename K, typename V, typename H, typename E>
void Map<K, V, H, E>::rehash(size_t newCap) {
  if (newCap < 16)
    newCap = 16;
  DynArray<BucketList> newBuckets(newCap);
  for (size_t b = 0; b < buckets_.size(); ++b) {
    while (!buckets_[b].empty()) {
      typename BucketList::Iterator it = buckets_[b].begin();
      Entry e = bmin::move(*it);
      buckets_[b].erase(it);
      size_t h = hasher_(e.key) % newCap;
      newBuckets[h].push_back(bmin::move(e));
    }
  }
  buckets_ = bmin::move(newBuckets);
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::findIterator(
    const K& key) {
  if (buckets_.empty())
    return end();
  size_t b = bucketIndex(key);
  for (typename BucketList::Iterator it = buckets_[b].begin();
       it != buckets_[b].end(); ++it) {
    if (equal_((*it).key, key))
      return Iterator(this, b, it);
  }
  return end();
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::begin() {
  if (buckets_.size() == 0)
    return end();
  Iterator it(this, 0, buckets_[0].begin());
  it.advancePastEmpty();
  return it;
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::end() const {
  return Iterator(const_cast<Map*>(this), buckets_.size(),
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
  if (buckets_.size() == 0)
    buckets_ = DynArray<BucketList>(16);
  if (size_ >= buckets_.size() * 3 / 4)
    rehash(buckets_.size() * 2);

  Iterator it = findIterator(key);
  if (it != end()) {
    (*it).value = bmin::move(value);
    return false;
  }

  size_t b = bucketIndex(key);
  buckets_[b].push_back(Entry{bmin::move(key), bmin::move(value)});
  ++size_;
  return true;
}

template <typename K, typename V, typename H, typename E>
V& Map<K, V, H, E>::operator[](const K& key) {
  Iterator it = findIterator(key);
  if (it != end())
    return (*it).value;
  insert(key, V{});
  return (*findIterator(key)).value;
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::erase(Iterator it) {
  if (it == end())
    return it;
  size_t b = it.bucket_;
  typename BucketList::Iterator next = it.inner_;
  ++next;
  buckets_[b].erase(it.inner_);
  --size_;
  Iterator out(this, b, next);
  out.advancePastEmpty();
  return out;
}

template <typename K, typename V, typename H, typename E>
bool Map<K, V, H, E>::erase(const K& key) {
  Iterator it = findIterator(key);
  if (it == end())
    return false;
  erase(it);
  return true;
}

}  // namespace bmin
