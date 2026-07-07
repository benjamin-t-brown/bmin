#pragma once

#include "internal/Types.h" // IWYU pragma: keep

namespace bmin {

template <typename T>
class DynArray;

class String {
  char* data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

  void growToFit(size_t minCapacity);

public:
  String();
  String(const char* s);
  String(const String& o);
  String(String&& o) noexcept;
  ~String();

  String& operator=(String o);

  size_t size() const {
    return size_;
  }

  bool empty() const {
    return size_ == 0;
  }

  const char* c_str() const {
    return data_ ? data_ : "";
  }

  char* data() {
    return data_;
  }

  const char* data() const {
    return data_;
  }

  void reserve(size_t n);
  void clear();

  String& append(const char* s);
  String& append(const char* s, size_t len);
  String& append(const String& s);

  String slice(size_t start) const;
  String slice(size_t start, size_t end) const;

  static String join(const String& sep, const String* parts, size_t count);
  static String join(const char* sep, const String* parts, size_t count);
  static String join(const String& sep, const DynArray<String>& parts);

  String& operator+=(const char* s) {
    return append(s);
  }

  String& operator+=(const String& s) {
    return append(s);
  }

  using iterator = char*;
  using const_iterator = const char*;

  iterator begin() {
    return data_;
  }

  iterator end() {
    return data_ + size_;
  }

  const_iterator begin() const {
    return data_;
  }

  const_iterator end() const {
    return data_ + size_;
  }
};

bool operator==(const String& a, const String& b);
bool operator==(const String& a, const char* b);
bool operator!=(const String& a, const String& b);
bool operator!=(const String& a, const char* b);

String operator+(String a, const String& b);
String operator+(String a, const char* b);

}  // namespace bmin
