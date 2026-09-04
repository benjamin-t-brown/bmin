module;
#include <cstddef>
#include <string_view>
#include "assert.h"

export module bmin.string;
export import bmin.core;
export import bmin.dynarray;

export namespace bmin {

class String {
  char* _data = nullptr;
  std::size_t _size = 0;
  std::size_t _capacity = 0;

  void growToFit(std::size_t minCapacity);

public:
  using value_type = char;
  static constexpr std::size_t npos = static_cast<std::size_t>(-1);

  String();
  String(const char* s);
  String(const char* s, std::size_t len);
  String(std::size_t count, char c);
  String(const String& o);
  String(String&& o) noexcept;
  ~String();

  String& operator=(String o);

  std::size_t size() const {
    return _size;
  }

  std::size_t length() const {
    return _size;
  }

  std::size_t capacity() const {
    return _capacity;
  }

  bool empty() const {
    return _size == 0;
  }

  const char* cStr() const {
    return _data ? _data : "";
  }

  const char* c_str() const {
    return cStr();
  }

  char* data() {
    return _data;
  }

  const char* data() const {
    return _data;
  }

  char& operator[](std::size_t i) {
    return _data[i];
  }

  const char& operator[](std::size_t i) const {
    return _data[i];
  }

  char& at(std::size_t i);
  const char& at(std::size_t i) const;

  void reserve(std::size_t n);
  void shrinkToFit();
  void clear();

  String& assign(const char* s);
  String& assign(const char* s, std::size_t len);
  String& assign(const String& s);

  String& append(const char* s);
  String& append(const char* s, std::size_t len);
  String& append(const String& s);
  String& append(char c);

  String& pushBack(char c) {
    return append(c);
  }

  String& push_back(char c) {
    return pushBack(c);
  }

  void popBack();
  void pop_back() {
    popBack();
  }

  String& insert(std::size_t pos, const char* s);
  String& insert(std::size_t pos, const char* s, std::size_t len);
  String& insert(std::size_t pos, const String& s);

  String& erase(std::size_t pos, std::size_t count = npos);

  String slice() const;
  String slice(std::ptrdiff_t start) const;
  String slice(std::ptrdiff_t start, std::ptrdiff_t end) const;
  std::string_view sliceView() const;
  std::string_view sliceView(std::ptrdiff_t start) const;
  std::string_view sliceView(std::ptrdiff_t start, std::ptrdiff_t end) const;
  String substr(std::size_t pos, std::size_t count = npos) const;

  std::size_t find(const char* needle, std::size_t start = 0) const;
  std::size_t find(const String& needle, std::size_t start = 0) const;

  bool contains(const char* needle) const;
  bool contains(const String& needle) const;
  bool startsWith(const char* prefix) const;
  bool endsWith(const char* suffix) const;

  int compare(const String& o) const;
  int compare(const char* s) const;

  static String join(const String& sep, const DynArray<String>& parts);
  static String join(const char* sep, const DynArray<String>& parts);

  DynArray<String> split(const char* sep) const;
  DynArray<String> split(const String& sep) const;

  static String fromInt(int value);

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
bool operator<(const char* a, const String& b);
bool operator>(const String& a, const String& b);
bool operator>(const String& a, const char* b);
bool operator>(const char* a, const String& b);
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

int parseInt(const String& s);
double parseDouble(const String& s);
bool isInt(const String& s);
bool isDouble(const String& s);

String to_string(std::size_t value);

}  // namespace bmin
