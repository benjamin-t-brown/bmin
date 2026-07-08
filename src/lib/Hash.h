#pragma once

#include "String.h"

#include "internal/Types.h"

namespace bmin {

namespace detail {

inline size_t fnv1aBytes(const unsigned char* bytes, size_t len) {
  size_t h = 2166136261u;
  for (size_t i = 0; i < len; ++i) {
    h ^= bytes[i];
    h *= 16777619u;
  }
  return h;
}

}  // namespace detail

template <typename T>
struct EqualTo {
  bool operator()(const T& a, const T& b) const {
    return a == b;
  }
};

template <typename T>
struct Hash;

template <>
struct Hash<int> {
  size_t operator()(int v) const {
    return static_cast<size_t>(v);
  }
};

template <>
struct Hash<String> {
  size_t operator()(const String& s) const {
    return detail::fnv1aBytes(reinterpret_cast<const unsigned char*>(s.data()),
                              s.size());
  }
};

template <>
struct Hash<const char*> {
  size_t operator()(const char* s) const {
    size_t h = 2166136261u;
    if (!s) {
      return h;
    }
    while (*s) {
      h ^= static_cast<unsigned char>(*s++);
      h *= 16777619u;
    }
    return h;
  }
};

// Default for trivially-copyable keys (POD structs, enums, pointers, …).
// Specialize Hash<T> for types that are not trivially copyable (e.g. String).
template <typename T>
struct Hash {
  size_t operator()(const T& v) const {
    static_assert(__is_trivially_copyable(T),
                  "bmin::Hash<T>: specialize bmin::Hash for non-trivially-"
                  "copyable key types");
    return detail::fnv1aBytes(reinterpret_cast<const unsigned char*>(&v),
                              sizeof(T));
  }
};

}  // namespace bmin
