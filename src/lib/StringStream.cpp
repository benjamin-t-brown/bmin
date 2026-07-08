#include "StringStream.h"

#include <stdio.h>

namespace bmin {

void StringStream::clear() {
  _buf.clear();
}

StringStream& StringStream::operator<<(const char* s) {
  if (s) {
    _buf.append(s);
  }
  return *this;
}

StringStream& StringStream::operator<<(const String& s) {
  _buf.append(s);
  return *this;
}

StringStream& StringStream::operator<<(int value) {
  _buf.append(String::fromInt(value));
  return *this;
}

StringStream& StringStream::operator<<(unsigned int value) {
  char digits[32];
  int len = snprintf(digits, sizeof(digits), "%u", value);
  if (len > 0) {
    _buf.append(digits, static_cast<size_t>(len));
  }
  return *this;
}

StringStream& StringStream::operator<<(size_t value) {
  if (value == 0) {
    _buf.append("0");
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

  _buf.append(digits, i);
  return *this;
}

StringStream& StringStream::operator<<(float value) {
  char digits[64];
  int len = snprintf(digits, sizeof(digits), "%g", value);
  if (len > 0) {
    _buf.append(digits, static_cast<size_t>(len));
  }
  return *this;
}

StringStream& StringStream::operator<<(double value) {
  char digits[64];
  int len = snprintf(digits, sizeof(digits), "%g", value);
  if (len > 0) {
    _buf.append(digits, static_cast<size_t>(len));
  }
  return *this;
}

StringStream& StringStream::operator<<(bool value) {
  _buf.append(value ? "true" : "false");
  return *this;
}

}  // namespace bmin
