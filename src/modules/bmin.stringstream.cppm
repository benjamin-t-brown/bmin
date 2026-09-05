module;
#include <cstddef>
#include <string_view>

export module bmin.stringstream;
export import bmin.string;

export namespace bmin {

class StringStream {
  String _buf;

public:
  void clear();

  String str() const {
    return _buf;
  }

  StringStream& operator<<(const char* s);
  StringStream& operator<<(const String& s);
  StringStream& operator<<(char value);
  StringStream& operator<<(int value);
  StringStream& operator<<(unsigned int value);
  StringStream& operator<<(std::size_t value);
  StringStream& operator<<(float value);
  StringStream& operator<<(double value);
  StringStream& operator<<(bool value);
  StringStream& operator<<(std::string_view value);
  StringStream& operator<<(const void* value);
};

}  // namespace bmin
