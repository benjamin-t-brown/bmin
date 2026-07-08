#pragma once

#include "internal/Types.h"  // IWYU pragma: keep

namespace bmin {

template <typename T>
class DynArray;

class String {
  char* data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

  void growToFit(size_t minCapacity);

public:
  static constexpr size_t npos = static_cast<size_t>(-1);

  String();
  String(const char* s);
  String(const char* s, size_t len);
  String(const String& o);
  String(String&& o) noexcept;
  ~String();

  String& operator=(String o);

  size_t size() const {
    return size_;
  }

  size_t length() const {
    return size_;
  }

  size_t capacity() const {
    return capacity_;
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

  char& operator[](size_t i) {
    return data_[i];
  }

  const char& operator[](size_t i) const {
    return data_[i];
  }

  char& at(size_t i);
  const char& at(size_t i) const;

  void reserve(size_t n);
  void shrink_to_fit();
  void clear();

  String& assign(const char* s);
  String& assign(const char* s, size_t len);
  String& assign(const String& s);

  String& append(const char* s);
  String& append(const char* s, size_t len);
  String& append(const String& s);
  String& append(char c);

  String& push_back(char c) {
    return append(c);
  }

  String& insert(size_t pos, const char* s);
  String& insert(size_t pos, const char* s, size_t len);
  String& insert(size_t pos, const String& s);

  String& erase(size_t pos, size_t count = npos);

  String slice(size_t start) const;
  String slice(size_t start, size_t end) const;
  String substr(size_t pos, size_t count = npos) const;

  size_t find(const char* needle, size_t start = 0) const;
  size_t find(const String& needle, size_t start = 0) const;

  bool contains(const char* needle) const;
  bool starts_with(const char* prefix) const;
  bool ends_with(const char* suffix) const;

  int compare(const String& o) const;
  int compare(const char* s) const;

  static String join(const String& sep, const String* parts, size_t count);
  static String join(const char* sep, const String* parts, size_t count);
  static String join(const String& sep, const DynArray<String>& parts);

  static String fromInt(int value);
  bool parseInt(int& out) const;

  String& operator+=(const char* s) {
    return append(s);
  }

  String& operator+=(const String& s) {
    return append(s);
  }

  String& operator+=(char c) {
    return append(c);
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
bool operator==(const char* a, const String& b);
bool operator!=(const String& a, const String& b);
bool operator!=(const String& a, const char* b);
bool operator!=(const char* a, const String& b);

bool operator<(const String& a, const String& b);
bool operator<(const String& a, const char* b);
bool operator>(const String& a, const String& b);
bool operator>(const String& a, const char* b);
bool operator<=(const String& a, const String& b);
bool operator>=(const String& a, const String& b);

String operator+(String a, const String& b);
String operator+(String a, const char* b);
String operator+(const char* a, const String& b);
String operator+(String a, char c);
String operator+(char c, const String& b);

String to_string(int value);
String to_string(long value);
String to_string(long long value);
String to_string(unsigned value);
String to_string(unsigned long value);
String to_string(unsigned long long value);
String to_string(float value);
String to_string(double value);
String to_string(long double value);

}  // namespace bmin
