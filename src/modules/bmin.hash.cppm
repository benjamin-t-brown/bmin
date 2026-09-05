module;
#include <cstddef>
#include <string_view>

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

template <>
struct EqualTo<String> {
  bool operator()(const String& a, const String& b) const {
    return a == b;
  }

  bool operator()(const String& a, std::string_view b) const {
    if (a.size() != b.size()) {
      return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
      if (a[i] != b[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator()(std::string_view a, const String& b) const {
    return (*this)(b, a);
  }

  bool operator()(const String& a, const char* b) const {
    return a == b;
  }

  bool operator()(const char* a, const String& b) const {
    return b == a;
  }
};

template <typename T, bool IsEnum = __is_enum(T)>
struct Hash;

namespace detail {

template <typename T>
struct IntegerHash {
  std::size_t operator()(T v) const {
    return static_cast<std::size_t>(v);
  }
};

}  // namespace detail

template <>
struct Hash<bool> : detail::IntegerHash<bool> {};
template <>
struct Hash<char> : detail::IntegerHash<char> {};
template <>
struct Hash<wchar_t> : detail::IntegerHash<wchar_t> {};
template <>
struct Hash<char8_t> : detail::IntegerHash<char8_t> {};
template <>
struct Hash<char16_t> : detail::IntegerHash<char16_t> {};
template <>
struct Hash<char32_t> : detail::IntegerHash<char32_t> {};
template <>
struct Hash<signed char> : detail::IntegerHash<signed char> {};
template <>
struct Hash<unsigned char> : detail::IntegerHash<unsigned char> {};
template <>
struct Hash<short> : detail::IntegerHash<short> {};
template <>
struct Hash<unsigned short> : detail::IntegerHash<unsigned short> {};
template <>
struct Hash<int> : detail::IntegerHash<int> {};
template <>
struct Hash<unsigned int> : detail::IntegerHash<unsigned int> {};
template <>
struct Hash<long> : detail::IntegerHash<long> {};
template <>
struct Hash<unsigned long> : detail::IntegerHash<unsigned long> {};
template <>
struct Hash<long long> : detail::IntegerHash<long long> {};
template <>
struct Hash<unsigned long long> : detail::IntegerHash<unsigned long long> {};

template <typename T>
struct Hash<T*, false> {
  std::size_t operator()(T* value) const {
    return reinterpret_cast<std::size_t>(value);
  }
};

template <typename T>
struct Hash<T, true> {
  std::size_t operator()(T value) const {
    using Underlying = __underlying_type(T);
    return Hash<Underlying>{}(static_cast<Underlying>(value));
  }
};

template <>
struct Hash<String> {
  std::size_t operator()(const String& s) const {
    return detail::fnv1aBytes(reinterpret_cast<const unsigned char*>(s.data()),
                              s.size());
  }

  std::size_t operator()(std::string_view s) const {
    return detail::fnv1aBytes(
        reinterpret_cast<const unsigned char*>(s.data()), s.size());
  }

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

// Custom structures require an explicit specialization so their hash matches
// their equality operation and never depends on padding bytes.
template <typename T, bool IsEnum>
struct Hash {
  std::size_t operator()(const T& v) const {
    (void)v;
    static_assert(sizeof(T) == 0,
                  "bmin::Hash<T>: specialize bmin::Hash for custom key types");
    return 0;
  }
};

}  // namespace bmin
