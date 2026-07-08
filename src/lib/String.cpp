#include "String.h"

#include "DynArray.h"
#include "./internal/Policy.h"
#include "./internal/Utility.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <cerrno>
#include <cfloat>
#include <climits>

namespace bmin {

namespace {

size_t cstrLen(const char* s) {
  size_t n = 0;
  if (!s) {
    return 0;
  }
  while (s[n] != '\0') {
    ++n;
  }
  return n;
}

void bytesCopy(char* dst, const char* src, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    dst[i] = src[i];
  }
}

int bytesCompare(const char* a, size_t an, const char* b, size_t bn) {
  size_t n = an < bn ? an : bn;
  for (size_t i = 0; i < n; ++i) {
    if (a[i] != b[i]) {
      return static_cast<unsigned char>(a[i]) -
             static_cast<unsigned char>(b[i]);
    }
  }
  if (an < bn) {
    return -1;
  }
  if (an > bn) {
    return 1;
  }
  return 0;
}

String stringFromFormat(const char* fmt, ...) {
  char buf[64];
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  if (len <= 0) {
    return String();
  }
  if (static_cast<size_t>(len) >= sizeof(buf)) {
    size_t cap = static_cast<size_t>(len) + 1;
    char* big = new char[cap];
    va_list args2;
    va_start(args2, fmt);
    vsnprintf(big, cap, fmt, args2);
    va_end(args2);
    String out;
    out.append(big, static_cast<size_t>(len));
    delete[] big;
    return out;
  }
  return String(buf);
}

bool parseIntValue(const char* start, int& out) {
  if (*start == '\0') {
    return false;
  }

  errno = 0;
  char* end = nullptr;
  const long value = strtol(start, &end, 10);
  if (errno == ERANGE || end == start || *end != '\0') {
    return false;
  }
  if (value < INT_MIN || value > INT_MAX) {
    return false;
  }

  out = static_cast<int>(value);
  return true;
}

bool parseDoubleValue(const char* start, double& out) {
  if (*start == '\0') {
    return false;
  }

  errno = 0;
  char* end = nullptr;
  const double value = strtod(start, &end);
  if (errno == ERANGE || end == start || *end != '\0') {
    return false;
  }

  out = value;
  return true;
}

} // namespace

void String::growToFit(size_t minCapacity) {
  if (minCapacity <= _capacity) {
    return;
  }
  size_t newCap = _capacity ? _capacity : 1;
  while (newCap < minCapacity) {
    newCap *= 2;
  }
  char* buf = new char[newCap + 1];
  if (_data) {
    bytesCopy(buf, _data, _size);
    delete[] _data;
  }
  buf[_size] = '\0';
  _data = buf;
  _capacity = newCap;
}

String::String() = default;

String::String(const char* s) {
  if (!s || !*s) {
    return;
  }
  _size = cstrLen(s);
  growToFit(_size);
  bytesCopy(_data, s, _size);
  _data[_size] = '\0';
}

String::String(const char* s, size_t len) {
  if (!s || len == 0) {
    return;
  }
  _size = len;
  growToFit(_size);
  bytesCopy(_data, s, _size);
  _data[_size] = '\0';
}

String::String(const String& o) {
  if (o._size == 0) {
    return;
  }
  _size = o._size;
  growToFit(_size);
  bytesCopy(_data, o._data, _size);
  _data[_size] = '\0';
}

String::String(String&& o) noexcept
    : _data(o._data), _size(o._size), _capacity(o._capacity) {
  o._data = nullptr;
  o._size = 0;
  o._capacity = 0;
}

String::~String() { delete[] _data; }

String& String::operator=(String o) {
  bmin::detail::swap(_data, o._data);
  bmin::detail::swap(_size, o._size);
  bmin::detail::swap(_capacity, o._capacity);
  return *this;
}

void String::reserve(size_t n) {
  if (n > _capacity) {
    growToFit(n);
  }
}

void String::clear() {
  _size = 0;
  if (_data) {
    _data[0] = '\0';
  }
}

void String::shrinkToFit() {
  if (_size == _capacity) {
    return;
  }
  if (_size == 0) {
    delete[] _data;
    _data = nullptr;
    _capacity = 0;
    return;
  }
  char* buf = new char[_size + 1];
  bytesCopy(buf, _data, _size);
  buf[_size] = '\0';
  delete[] _data;
  _data = buf;
  _capacity = _size;
}

char& String::at(size_t i) {
  BMIN_ASSERT(i < _size);
  return _data[i];
}

const char& String::at(size_t i) const {
  BMIN_ASSERT(i < _size);
  return _data[i];
}

String& String::assign(const char* s) {
  clear();
  return append(s);
}

String& String::assign(const char* s, size_t len) {
  clear();
  return append(s, len);
}

String& String::assign(const String& s) {
  clear();
  return append(s);
}

String& String::append(const char* s) {
  if (!s || !*s) {
    return *this;
  }
  return append(s, cstrLen(s));
}

String& String::append(const char* s, size_t len) {
  if (!s || len == 0) {
    return *this;
  }
  size_t newSize = _size + len;
  growToFit(newSize);
  bytesCopy(_data + _size, s, len);
  _size = newSize;
  _data[_size] = '\0';
  return *this;
}

String& String::append(const String& s) { return append(s.data(), s.size()); }

String& String::append(char c) { return append(&c, 1); }

String& String::insert(size_t pos, const char* s, size_t len) {
  if (pos > _size) {
    pos = _size;
  }
  if (!s || len == 0) {
    return *this;
  }
  size_t oldSize = _size;
  size_t newSize = oldSize + len;
  growToFit(newSize);
  for (size_t i = oldSize; i > pos; --i) {
    _data[i + len - 1] = _data[i - 1];
  }
  bytesCopy(_data + pos, s, len);
  _size = newSize;
  _data[_size] = '\0';
  return *this;
}

String& String::insert(size_t pos, const char* s) {
  return insert(pos, s, cstrLen(s));
}

String& String::insert(size_t pos, const String& s) {
  return insert(pos, s.data(), s.size());
}

String& String::erase(size_t pos, size_t count) {
  if (pos >= _size) {
    return *this;
  }
  size_t maxRemove = _size - pos;
  if (count > maxRemove) {
    count = maxRemove;
  }
  size_t tailStart = pos + count;
  size_t tailLen = _size - tailStart;
  for (size_t i = 0; i < tailLen; ++i) {
    _data[pos + i] = _data[tailStart + i];
  }
  _size -= count;
  if (_data) {
    _data[_size] = '\0';
  }
  return *this;
}

namespace {

struct SliceBounds {
  size_t start = 0;
  size_t end = 0;
};

SliceBounds sliceBounds(size_t size, ptrdiff_t start, ptrdiff_t end, bool hasEnd) {
  ptrdiff_t len = static_cast<ptrdiff_t>(size);
  ptrdiff_t s = start;
  ptrdiff_t e = hasEnd ? end : len;

  if (s < 0) {
    s = len + s;
    if (s < 0) {
      s = 0;
    }
  } else if (s > len) {
    s = len;
  }

  if (e < 0) {
    e = len + e;
    if (e < 0) {
      e = 0;
    }
  } else if (e > len) {
    e = len;
  }

  SliceBounds bounds;
  if (s > e) {
    return bounds;
  }
  bounds.start = static_cast<size_t>(s);
  bounds.end = static_cast<size_t>(e);
  return bounds;
}

}  // namespace

String String::slice() const {
  return String(*this);
}

String String::slice(ptrdiff_t start) const {
  SliceBounds bounds = sliceBounds(_size, start, 0, false);
  if (bounds.start >= bounds.end) {
    return String();
  }
  String out;
  size_t len = bounds.end - bounds.start;
  out.reserve(len);
  out.append(cStr() + bounds.start);
  out._size = len;
  if (out._data) {
    out._data[len] = '\0';
  }
  return out;
}

String String::slice(ptrdiff_t start, ptrdiff_t end) const {
  SliceBounds bounds = sliceBounds(_size, start, end, true);
  if (bounds.start >= bounds.end) {
    return String();
  }
  String out;
  size_t len = bounds.end - bounds.start;
  out.reserve(len);
  out.append(cStr() + bounds.start);
  out._size = len;
  if (out._data) {
    out._data[len] = '\0';
  }
  return out;
}

std::string_view String::sliceView() const {
  return std::string_view(cStr(), _size);
}

std::string_view String::sliceView(ptrdiff_t start) const {
  SliceBounds bounds = sliceBounds(_size, start, 0, false);
  if (bounds.start >= bounds.end) {
    return std::string_view();
  }
  return std::string_view(cStr() + bounds.start, bounds.end - bounds.start);
}

std::string_view String::sliceView(ptrdiff_t start, ptrdiff_t end) const {
  SliceBounds bounds = sliceBounds(_size, start, end, true);
  if (bounds.start >= bounds.end) {
    return std::string_view();
  }
  return std::string_view(cStr() + bounds.start, bounds.end - bounds.start);
}

String String::substr(size_t pos, size_t count) const {
  if (pos >= _size) {
    return String();
  }
  size_t len = _size - pos;
  if (count < len) {
    len = count;
  }
  return slice(pos, pos + len);
}

size_t String::find(const char* needle, size_t start) const {
  if (!needle || !*needle || start >= _size) {
    return npos;
  }
  size_t nlen = cstrLen(needle);
  if (nlen == 0 || start + nlen > _size) {
    return npos;
  }
  for (size_t i = start; i + nlen <= _size; ++i) {
    bool match = true;
    for (size_t j = 0; j < nlen; ++j) {
      if (_data[i + j] != needle[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return i;
    }
  }
  return npos;
}

size_t String::find(const String& needle, size_t start) const {
  return find(needle.cStr(), start);
}

bool String::contains(const char* needle) const { return find(needle) != npos; }

bool String::startsWith(const char* prefix) const {
  if (!prefix) {
    return true;
  }
  size_t plen = cstrLen(prefix);
  if (plen > _size) {
    return false;
  }
  return bytesCompare(_data, plen, prefix, plen) == 0;
}

bool String::endsWith(const char* suffix) const {
  if (!suffix) {
    return true;
  }
  size_t slen = cstrLen(suffix);
  if (slen > _size) {
    return false;
  }
  return bytesCompare(_data + _size - slen, slen, suffix, slen) == 0;
}

int String::compare(const String& o) const {
  return bytesCompare(_data ? _data : "", _size, o.data(), o._size);
}

int String::compare(const char* s) const {
  if (!s) {
    s = "";
  }
  return bytesCompare(_data ? _data : "", _size, s, cstrLen(s));
}

String String::join(const char* sep, const DynArray<String>& parts) {
  String out;
  if (parts.size() == 0) {
    return out;
  }
  size_t sepLen = cstrLen(sep);
  size_t total = parts[0].size();
  for (size_t i = 1; i < parts.size(); ++i) {
    total += sepLen + parts[i].size();
  }
  out.reserve(total);
  out.append(parts[0].cStr());
  for (size_t i = 1; i < parts.size(); ++i) {
    if (sepLen) {
      out.append(sep);
    }
    out.append(parts[i].cStr());
  }
  return out;
}

String String::join(const String& sep, const DynArray<String>& parts) {
  return join(sep.cStr(), parts);
}

DynArray<String> String::split(const char* sep) const {
  DynArray<String> out;
  if (!sep || !*sep) {
    out.pushBack(*this);
    return out;
  }
  if (_size == 0) {
    out.pushBack(String());
    return out;
  }
  size_t sepLen = cstrLen(sep);
  size_t start = 0;
  while (true) {
    size_t pos = find(sep, start);
    if (pos == npos) {
      out.pushBack(slice(start));
      break;
    }
    out.pushBack(slice(start, pos));
    start = pos + sepLen;
  }
  return out;
}

DynArray<String> String::split(const String& sep) const {
  return split(sep.cStr());
}

String String::fromInt(int value) {
  if (value == 0) {
    return String("0");
  }

  char buf[16];
  size_t i = 0;
  unsigned magnitude;
  if (value < 0) {
    buf[i++] = '-';
    magnitude = static_cast<unsigned>(-(static_cast<long long>(value)));
  } else {
    magnitude = static_cast<unsigned>(value);
  }

  const size_t digitsStart = i;
  while (magnitude != 0) {
    buf[i++] = static_cast<char>('0' + (magnitude % 10));
    magnitude /= 10;
  }

  for (size_t l = digitsStart, r = i - 1; l < r; ++l, --r) {
    char tmp = buf[l];
    buf[l] = buf[r];
    buf[r] = tmp;
  }

  buf[i] = '\0';
  return String(buf);
}

int parseInt(const String& s) {
  int out = 0;
  if (!parseIntValue(s.cStr(), out)) {
    return INT_MAX;
  }
  return out;
}

double parseDouble(const String& s) {
  double out = 0.0;
  if (!parseDoubleValue(s.cStr(), out)) {
    return DBL_MAX;
  }
  return out;
}

bool isInt(const String& s) {
  int out = 0;
  return parseIntValue(s.cStr(), out);
}

bool isDouble(const String& s) {
  double out = 0.0;
  return parseDoubleValue(s.cStr(), out);
}

bool operator==(const String& a, const String& b) { return a.compare(b) == 0; }

bool operator==(const String& a, const char* b) { return a.compare(b) == 0; }

bool operator==(const char* a, const String& b) { return b.compare(a) == 0; }

bool operator!=(const String& a, const String& b) { return !(a == b); }

bool operator!=(const String& a, const char* b) { return !(a == b); }

bool operator!=(const char* a, const String& b) { return !(a == b); }

bool operator<(const String& a, const String& b) { return a.compare(b) < 0; }

bool operator<(const String& a, const char* b) { return a.compare(b) < 0; }

bool operator>(const String& a, const String& b) { return a.compare(b) > 0; }

bool operator>(const String& a, const char* b) { return a.compare(b) > 0; }

bool operator<=(const String& a, const String& b) { return a.compare(b) <= 0; }

bool operator>=(const String& a, const String& b) { return a.compare(b) >= 0; }

String operator+(String a, const String& b) {
  a.append(b);
  return a;
}

String operator+(String a, const char* b) {
  a.append(b);
  return a;
}

String operator+(const char* a, const String& b) {
  String out(a ? a : "");
  out.append(b);
  return out;
}

String operator+(String a, char c) {
  a.append(c);
  return a;
}

String operator+(char c, const String& b) {
  String out;
  out.append(c);
  out.append(b);
  return out;
}

String toString(int value) { return String::fromInt(value); }

String toString(long value) { return stringFromFormat("%ld", value); }

String toString(long long value) { return stringFromFormat("%lld", value); }

String toString(unsigned value) { return stringFromFormat("%u", value); }

String toString(unsigned long value) { return stringFromFormat("%lu", value); }

String toString(unsigned long long value) {
  return stringFromFormat("%llu", value);
}

String toString(float value) {
  return stringFromFormat("%f", static_cast<double>(value));
}

String toString(double value) { return stringFromFormat("%f", value); }

String toString(long double value) { return stringFromFormat("%Lf", value); }

} // namespace bmin
