#pragma once

#include <string_view>

#include "String.h"

namespace bmin {

std::string_view toStringView(const String& s);

String fromStringView(std::string_view v);
void toStringView(String&&) = delete;

String& append(String& s, std::string_view v);
bool operator==(const String& a, std::string_view b);
bool operator==(std::string_view a, const String& b);
bool operator!=(const String& a, std::string_view b);
bool operator!=(std::string_view a, const String& b);
bool operator<(const String& a, std::string_view b);
bool operator<(std::string_view a, const String& b);
bool operator>(const String& a, std::string_view b);
bool operator>(std::string_view a, const String& b);
bool operator<=(const String& a, std::string_view b);
bool operator<=(std::string_view a, const String& b);
bool operator>=(const String& a, std::string_view b);
bool operator>=(std::string_view a, const String& b);

}  // namespace bmin
