#include <string_view>

import bmin.string_interop;

bool stringInteropImportProbe() {
  return bmin::String("direct") == std::string_view("direct");
}
