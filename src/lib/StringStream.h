#pragma once

#include "String.h"

namespace bmin {

class StringStream {
  String buf_;

public:
  void clear();

  String str() const {
    return buf_;
  }

  StringStream& operator<<(const char* s);
  StringStream& operator<<(const String& s);
  StringStream& operator<<(int value);
  StringStream& operator<<(unsigned int value);
  StringStream& operator<<(size_t value);
  StringStream& operator<<(float value);
  StringStream& operator<<(double value);
  StringStream& operator<<(bool value);
};

}  // namespace bmin
