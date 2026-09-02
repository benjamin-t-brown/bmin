module;
#include <cstddef>

export module bmin.map;
export import bmin.dynarray;
export import bmin.hash;
export import bmin.list;

export namespace bmin {

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
  std::size_t _size = 0;
  H _hasher{};
  E _equal{};

  std::size_t bucketIndex(const K& key) const;
  Iterator findIterator(const K& key);
  void rehash(std::size_t newCap);

public:
  Map();
  Map(const Map& o);
  Map(Map&& o) noexcept;
  Map& operator=(Map o);
  ~Map() = default;

  std::size_t size() const {
    return _size;
  }

  bool empty() const {
    return _size == 0;
  }

  Iterator begin() const;
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
  std::size_t _bucket = 0;
  typename BucketList::Iterator _inner{};

  void advancePastEmpty();

  Iterator(Map* m, std::size_t b, typename BucketList::Iterator it)
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
std::size_t Map<K, V, H, E>::bucketIndex(const K& key) const {
  return _buckets.size() ? _hasher(key) % _buckets.size() : 0;
}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map() : _buckets(16) {}

template <typename K, typename V, typename H, typename E>
Map<K, V, H, E>::Map(const Map& o) : _buckets(16), _size(0) {
  for (std::size_t b = 0; b < o._buckets.size(); ++b) {
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
void Map<K, V, H, E>::rehash(std::size_t newCap) {
  if (newCap < 16) {
    newCap = 16;
  }
  DynArray<BucketList> newBuckets(newCap);
  for (std::size_t b = 0; b < _buckets.size(); ++b) {
    while (!_buckets[b].empty()) {
      typename BucketList::Iterator it = _buckets[b].begin();
      Entry e = bmin::move(*it);
      _buckets[b].erase(it);
      std::size_t h = _hasher(e.key) % newCap;
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
  std::size_t b = bucketIndex(key);
  for (typename BucketList::Iterator it = _buckets[b].begin();
       it != _buckets[b].end(); ++it) {
    if (_equal((*it).key, key)) {
      return Iterator(this, b, it);
    }
  }
  return end();
}

template <typename K, typename V, typename H, typename E>
typename Map<K, V, H, E>::Iterator Map<K, V, H, E>::begin() const {
  if (_buckets.size() == 0) {
    return end();
  }
  Iterator it(const_cast<Map*>(this), 0, _buckets[0].begin());
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

  std::size_t b = bucketIndex(key);
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
  std::size_t b = it._bucket;
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
