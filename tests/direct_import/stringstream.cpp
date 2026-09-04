import bmin.stringstream;

auto stringStreamImportProbe() {
  bmin::StringStream stream;
  stream << 42;
  return stream.str();
}
