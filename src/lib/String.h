#pragma once

#include <string_view>

#include "./internal/Types.h"  // IWYU pragma: keep

namespace bmin {

template <typename T>
class DynArray;

class String {
  char* _data = nullptr;
  size_t _size = 0;
  size_t _capacity = 0;

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
    return _size;
  }

  size_t length() const {
    return _size;
  }

  size_t capacity() const {
    return _capacity;
  }

  bool empty() const {
    return _size == 0;
  }

  const char* cStr() const {
    return _data ? _data : "";
  }

  char* data() {
    return _data;
  }

  const char* data() const {
    return _data;
  }

  char& operator[](size_t i) {
    return _data[i];
  }

  const char& operator[](size_t i) const {
    return _data[i];
  }

  char& at(size_t i);
  const char& at(size_t i) const;

  void reserve(size_t n);
  void shrinkToFit();
  void clear();

  String& assign(const char* s);
  String& assign(const char* s, size_t len);
  String& assign(const String& s);

  String& append(const char* s);
  String& append(const char* s, size_t len);
  String& append(const String& s);
  String& append(char c);

  String& pushBack(char c) {
    return append(c);
  }

  String& insert(size_t pos, const char* s);
  String& insert(size_t pos, const char* s, size_t len);
  String& insert(size_t pos, const String& s);

  String& erase(size_t pos, size_t count = npos);

  String slice() const;
  String slice(ptrdiff_t start) const;
  String slice(ptrdiff_t start, ptrdiff_t end) const;
  std::string_view sliceView() const;
  std::string_view sliceView(ptrdiff_t start) const;
  std::string_view sliceView(ptrdiff_t start, ptrdiff_t end) const;
  String substr(size_t pos, size_t count = npos) const;

  size_t find(const char* needle, size_t start = 0) const;
  size_t find(const String& needle, size_t start = 0) const;

  bool contains(const char* needle) const;
  bool startsWith(const char* prefix) const;
  bool endsWith(const char* suffix) const;

  int compare(const String& o) const;
  int compare(const char* s) const;

  static String join(const String& sep, const DynArray<String>& parts);
  static String join(const char* sep, const DynArray<String>& parts);

  DynArray<String> split(const char* sep) const;
  DynArray<String> split(const String& sep) const;

  static String fromInt(int value);
  int parseInt() const;
  double parseDouble() const;
  bool isInt() const;
  bool isDouble() const;

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
    return _data;
  }

  iterator end() {
    return _data + _size;
  }

  const_iterator begin() const {
    return _data;
  }

  const_iterator end() const {
    return _data + _size;
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

String toString(int value);
String toString(long value);
String toString(long long value);
String toString(unsigned value);
String toString(unsigned long value);
String toString(unsigned long long value);
String toString(float value);
String toString(double value);
String toString(long double value);

}  // namespace bmin
