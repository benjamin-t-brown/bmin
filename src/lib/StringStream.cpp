#include "StringStream.h"

#include <stdio.h>

namespace bmin {

void StringStream::clear() {
  buf_.clear();
}

StringStream& StringStream::operator<<(const char* s) {
  if (s)
    buf_.append(s);
  return *this;
}

StringStream& StringStream::operator<<(const String& s) {
  buf_.append(s);
  return *this;
}

StringStream& StringStream::operator<<(int value) {
  buf_.append(String::fromInt(value));
  return *this;
}

StringStream& StringStream::operator<<(unsigned int value) {
  char digits[32];
  int len = snprintf(digits, sizeof(digits), "%u", value);
  if (len > 0)
    buf_.append(digits, static_cast<size_t>(len));
  return *this;
}

StringStream& StringStream::operator<<(size_t value) {
  if (value == 0) {
    buf_.append("0");
    return *this;
  }

  char digits[32];
  size_t i = 0;
  while (value != 0) {
    digits[i++] = static_cast<char>('0' + (value % 10));
    value /= 10;
  }

  for (size_t l = 0, r = i - 1; l < r; ++l, --r) {
    char tmp = digits[l];
    digits[l] = digits[r];
    digits[r] = tmp;
  }

  buf_.append(digits, i);
  return *this;
}

StringStream& StringStream::operator<<(float value) {
  char digits[64];
  int len = snprintf(digits, sizeof(digits), "%g", value);
  if (len > 0)
    buf_.append(digits, static_cast<size_t>(len));
  return *this;
}

StringStream& StringStream::operator<<(double value) {
  char digits[64];
  int len = snprintf(digits, sizeof(digits), "%g", value);
  if (len > 0)
    buf_.append(digits, static_cast<size_t>(len));
  return *this;
}

StringStream& StringStream::operator<<(bool value) {
  buf_.append(value ? "true" : "false");
  return *this;
}

}  // namespace bmin
