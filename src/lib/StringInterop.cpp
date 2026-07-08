#include "StringInterop.h"

namespace bmin {

std::string_view toStringView(const String& s) {
  return std::string_view(s.data(), s.size());
}

String& append(String& s, std::string_view v) {
  return s.append(v.data(), v.size());
}

bool operator==(const String& a, std::string_view b) {
  if (a.size() != b.size()) {
    return false;
  }
  return b == toStringView(a);
}

bool operator==(std::string_view a, const String& b) {
  return b == a;
}

bool operator!=(const String& a, std::string_view b) {
  return !(a == b);
}

bool operator!=(std::string_view a, const String& b) {
  return !(a == b);
}

bool operator<(const String& a, std::string_view b) {
  return toStringView(a) < b;
}

bool operator<(std::string_view a, const String& b) {
  return a < toStringView(b);
}

bool operator>(const String& a, std::string_view b) {
  return toStringView(a) > b;
}

bool operator>(std::string_view a, const String& b) {
  return a > toStringView(b);
}

bool operator<=(const String& a, std::string_view b) {
  return toStringView(a) <= b;
}

bool operator<=(std::string_view a, const String& b) {
  return a <= toStringView(b);
}

bool operator>=(const String& a, std::string_view b) {
  return toStringView(a) >= b;
}

bool operator>=(std::string_view a, const String& b) {
  return a >= toStringView(b);
}

}  // namespace bmin
