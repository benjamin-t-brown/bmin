#include "String.h"

#include "DynArray.h"
#include "internal/Policy.h"
#include "internal/Utility.h"

#include <stdarg.h>
#include <stdio.h>

namespace bmin {

namespace {

size_t cstrLen(const char* s) {
  size_t n = 0;
  if (!s)
    return 0;
  while (s[n] != '\0')
    ++n;
  return n;
}

void bytesCopy(char* dst, const char* src, size_t n) {
  for (size_t i = 0; i < n; ++i)
    dst[i] = src[i];
}

int bytesCompare(const char* a, size_t an, const char* b, size_t bn) {
  size_t n = an < bn ? an : bn;
  for (size_t i = 0; i < n; ++i) {
    if (a[i] != b[i])
      return static_cast<unsigned char>(a[i]) -
             static_cast<unsigned char>(b[i]);
  }
  if (an < bn)
    return -1;
  if (an > bn)
    return 1;
  return 0;
}

String stringFromFormat(const char* fmt, ...) {
  char buf[64];
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  if (len <= 0)
    return String();
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

}  // namespace

void String::growToFit(size_t minCapacity) {
  if (minCapacity <= capacity_)
    return;
  size_t newCap = capacity_ ? capacity_ : 1;
  while (newCap < minCapacity)
    newCap *= 2;
  char* buf = new char[newCap + 1];
  if (data_) {
    bytesCopy(buf, data_, size_);
    delete[] data_;
  }
  buf[size_] = '\0';
  data_ = buf;
  capacity_ = newCap;
}

String::String() = default;

String::String(const char* s) {
  if (!s || !*s)
    return;
  size_ = cstrLen(s);
  growToFit(size_);
  bytesCopy(data_, s, size_);
  data_[size_] = '\0';
}

String::String(const char* s, size_t len) {
  if (!s || len == 0)
    return;
  size_ = len;
  growToFit(size_);
  bytesCopy(data_, s, size_);
  data_[size_] = '\0';
}

String::String(const String& o) {
  if (o.size_ == 0)
    return;
  size_ = o.size_;
  growToFit(size_);
  bytesCopy(data_, o.data_, size_);
  data_[size_] = '\0';
}

String::String(String&& o) noexcept
    : data_(o.data_), size_(o.size_), capacity_(o.capacity_) {
  o.data_ = nullptr;
  o.size_ = 0;
  o.capacity_ = 0;
}

String::~String() {
  delete[] data_;
}

String& String::operator=(String o) {
  bmin::swap(data_, o.data_);
  bmin::swap(size_, o.size_);
  bmin::swap(capacity_, o.capacity_);
  return *this;
}

void String::reserve(size_t n) {
  if (n > capacity_)
    growToFit(n);
}

void String::clear() {
  size_ = 0;
  if (data_)
    data_[0] = '\0';
}

void String::shrink_to_fit() {
  if (size_ == capacity_)
    return;
  if (size_ == 0) {
    delete[] data_;
    data_ = nullptr;
    capacity_ = 0;
    return;
  }
  char* buf = new char[size_ + 1];
  bytesCopy(buf, data_, size_);
  buf[size_] = '\0';
  delete[] data_;
  data_ = buf;
  capacity_ = size_;
}

char& String::at(size_t i) {
  BMIN_ASSERT(i < size_);
  return data_[i];
}

const char& String::at(size_t i) const {
  BMIN_ASSERT(i < size_);
  return data_[i];
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
  if (!s || !*s)
    return *this;
  return append(s, cstrLen(s));
}

String& String::append(const char* s, size_t len) {
  if (!s || len == 0)
    return *this;
  size_t newSize = size_ + len;
  growToFit(newSize);
  bytesCopy(data_ + size_, s, len);
  size_ = newSize;
  data_[size_] = '\0';
  return *this;
}

String& String::append(const String& s) {
  return append(s.data(), s.size());
}

String& String::append(char c) {
  return append(&c, 1);
}

String& String::insert(size_t pos, const char* s, size_t len) {
  if (pos > size_)
    pos = size_;
  if (!s || len == 0)
    return *this;
  size_t oldSize = size_;
  size_t newSize = oldSize + len;
  growToFit(newSize);
  for (size_t i = oldSize; i > pos; --i)
    data_[i + len - 1] = data_[i - 1];
  bytesCopy(data_ + pos, s, len);
  size_ = newSize;
  data_[size_] = '\0';
  return *this;
}

String& String::insert(size_t pos, const char* s) {
  return insert(pos, s, cstrLen(s));
}

String& String::insert(size_t pos, const String& s) {
  return insert(pos, s.data(), s.size());
}

String& String::erase(size_t pos, size_t count) {
  if (pos >= size_)
    return *this;
  size_t maxRemove = size_ - pos;
  if (count > maxRemove)
    count = maxRemove;
  size_t tailStart = pos + count;
  size_t tailLen = size_ - tailStart;
  for (size_t i = 0; i < tailLen; ++i)
    data_[pos + i] = data_[tailStart + i];
  size_ -= count;
  if (data_)
    data_[size_] = '\0';
  return *this;
}

String String::slice(size_t start) const {
  if (start >= size_)
    return String();
  String out;
  out.append(c_str() + start);
  return out;
}

String String::slice(size_t start, size_t end) const {
  if (start >= end || start >= size_)
    return String();
  if (end > size_)
    end = size_;
  String out;
  size_t len = end - start;
  out.reserve(len);
  out.append(c_str() + start);
  out.size_ = len;
  if (out.data_)
    out.data_[len] = '\0';
  return out;
}

String String::substr(size_t pos, size_t count) const {
  if (pos >= size_)
    return String();
  size_t len = size_ - pos;
  if (count < len)
    len = count;
  return slice(pos, pos + len);
}

size_t String::find(const char* needle, size_t start) const {
  if (!needle || !*needle || start >= size_)
    return npos;
  size_t nlen = cstrLen(needle);
  if (nlen == 0 || start + nlen > size_)
    return npos;
  for (size_t i = start; i + nlen <= size_; ++i) {
    bool match = true;
    for (size_t j = 0; j < nlen; ++j) {
      if (data_[i + j] != needle[j]) {
        match = false;
        break;
      }
    }
    if (match)
      return i;
  }
  return npos;
}

size_t String::find(const String& needle, size_t start) const {
  return find(needle.c_str(), start);
}

bool String::contains(const char* needle) const {
  return find(needle) != npos;
}

bool String::starts_with(const char* prefix) const {
  if (!prefix)
    return true;
  size_t plen = cstrLen(prefix);
  if (plen > size_)
    return false;
  return bytesCompare(data_, plen, prefix, plen) == 0;
}

bool String::ends_with(const char* suffix) const {
  if (!suffix)
    return true;
  size_t slen = cstrLen(suffix);
  if (slen > size_)
    return false;
  return bytesCompare(data_ + size_ - slen, slen, suffix, slen) == 0;
}

int String::compare(const String& o) const {
  return bytesCompare(data_ ? data_ : "", size_, o.data(), o.size_);
}

int String::compare(const char* s) const {
  if (!s)
    s = "";
  return bytesCompare(data_ ? data_ : "", size_, s, cstrLen(s));
}

String String::join(const String& sep, const String* parts, size_t count) {
  return join(sep.c_str(), parts, count);
}

String String::join(const char* sep, const String* parts, size_t count) {
  String out;
  if (!parts || count == 0)
    return out;
  size_t sepLen = cstrLen(sep);
  size_t total = parts[0].size();
  for (size_t i = 1; i < count; ++i)
    total += sepLen + parts[i].size();
  out.reserve(total);
  out.append(parts[0].c_str());
  for (size_t i = 1; i < count; ++i) {
    if (sepLen)
      out.append(sep);
    out.append(parts[i].c_str());
  }
  return out;
}

String String::join(const String& sep, const DynArray<String>& parts) {
  if (parts.size() == 0)
    return String();
  return join(sep, parts.data(), parts.size());
}

String String::fromInt(int value) {
  if (value == 0)
    return String("0");

  char buf[16];
  size_t i = 0;
  unsigned magnitude;
  if (value < 0) {
    buf[i++] = '-';
    magnitude =
        static_cast<unsigned>(-(static_cast<long long>(value)));
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

bool String::parseInt(int& out) const {
  const char* p = c_str();
  if (*p == '\0')
    return false;

  bool negative = false;
  if (*p == '+') {
    ++p;
  } else if (*p == '-') {
    negative = true;
    ++p;
  }

  if (*p == '\0')
    return false;

  long long value = 0;
  while (*p != '\0') {
    if (*p < '0' || *p > '9')
      return false;
    value = value * 10 + (*p - '0');
    ++p;
  }

  if (negative)
    value = -value;

  if (value < -2147483648LL || value > 2147483647LL)
    return false;

  out = static_cast<int>(value);
  return true;
}

bool operator==(const String& a, const String& b) {
  return a.compare(b) == 0;
}

bool operator==(const String& a, const char* b) {
  return a.compare(b) == 0;
}

bool operator==(const char* a, const String& b) {
  return b.compare(a) == 0;
}

bool operator!=(const String& a, const String& b) {
  return !(a == b);
}

bool operator!=(const String& a, const char* b) {
  return !(a == b);
}

bool operator!=(const char* a, const String& b) {
  return !(a == b);
}

bool operator<(const String& a, const String& b) {
  return a.compare(b) < 0;
}

bool operator<(const String& a, const char* b) {
  return a.compare(b) < 0;
}

bool operator>(const String& a, const String& b) {
  return a.compare(b) > 0;
}

bool operator>(const String& a, const char* b) {
  return a.compare(b) > 0;
}

bool operator<=(const String& a, const String& b) {
  return a.compare(b) <= 0;
}

bool operator>=(const String& a, const String& b) {
  return a.compare(b) >= 0;
}

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

String to_string(int value) {
  return String::fromInt(value);
}

String to_string(long value) {
  return stringFromFormat("%ld", value);
}

String to_string(long long value) {
  return stringFromFormat("%lld", value);
}

String to_string(unsigned value) {
  return stringFromFormat("%u", value);
}

String to_string(unsigned long value) {
  return stringFromFormat("%lu", value);
}

String to_string(unsigned long long value) {
  return stringFromFormat("%llu", value);
}

String to_string(float value) {
  return stringFromFormat("%f", static_cast<double>(value));
}

String to_string(double value) {
  return stringFromFormat("%f", value);
}

String to_string(long double value) {
  return stringFromFormat("%Lf", value);
}

}  // namespace bmin
