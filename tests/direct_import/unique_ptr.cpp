import bmin.unique_ptr;

int uniquePtrImportProbe() {
  auto value = bmin::makeUnique<int>(42);
  return *value;
}
