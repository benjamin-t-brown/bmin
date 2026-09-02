module;
#include <cstddef>

export module bmin.hash;
export import bmin.string;

export namespace bmin {

namespace detail {

inline std::size_t fnv1aBytes(const unsigned char* bytes, std::size_t len) {
  std::size_t h = 2166136261u;
  for (std::size_t i = 0; i < len; ++i) {
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
  std::size_t operator()(int v) const {
    return static_cast<std::size_t>(v);
  }
};

template <>
struct Hash<String> {
  std::size_t operator()(const String& s) const {
    return detail::fnv1aBytes(reinterpret_cast<const unsigned char*>(s.data()),
                              s.size());
  }
};

template <>
struct Hash<const char*> {
  std::size_t operator()(const char* s) const {
    std::size_t h = 2166136261u;
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
template <typename T>
struct Hash {
  std::size_t operator()(const T& v) const {
    static_assert(__is_trivially_copyable(T),
                  "bmin::Hash<T>: specialize bmin::Hash for non-trivially-"
                  "copyable key types");
    return detail::fnv1aBytes(reinterpret_cast<const unsigned char*>(&v),
                              sizeof(T));
  }
};

}  // namespace bmin
