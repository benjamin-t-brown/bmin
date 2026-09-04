import bmin.map;

int mapImportProbe() {
  bmin::Map<int, int> values;
  values.insert(1, 2);
  return values[1];
}
