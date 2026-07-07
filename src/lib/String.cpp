#include "String.h"

#include "DynArray.h"
#include "internal/Utility.h"

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

int cstrCmp(const char* a, const char* b) {
  if (!a)
    a = "";
  if (!b)
    b = "";
  while (*a && *a == *b) {
    ++a;
    ++b;
  }
  return static_cast<unsigned char>(*a) -
         static_cast<unsigned char>(*b);
}

void bytesCopy(char* dst, const char* src, size_t n) {
  for (size_t i = 0; i < n; ++i)
    dst[i] = src[i];
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

String::String(const String& o) : String(o.c_str()) {}

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

bool operator==(const String& a, const String& b) {
  if (a.size() != b.size())
    return false;
  return cstrCmp(a.c_str(), b.c_str()) == 0;
}

bool operator==(const String& a, const char* b) {
  return cstrCmp(a.c_str(), b ? b : "") == 0;
}

bool operator!=(const String& a, const String& b) {
  return !(a == b);
}

bool operator!=(const String& a, const char* b) {
  return !(a == b);
}

String operator+(String a, const String& b) {
  a.append(b);
  return a;
}

String operator+(String a, const char* b) {
  a.append(b);
  return a;
}

}  // namespace bmin
