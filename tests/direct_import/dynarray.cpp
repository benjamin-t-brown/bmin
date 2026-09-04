import bmin.dynarray;

int dynArrayImportProbe() {
  bmin::DynArray<int> values;
  values.pushBack(1);
  return values[0];
}
