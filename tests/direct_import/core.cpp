import bmin.core;

int coreImportProbe() {
  int value = 1;
  return bmin::exchange(value, 2);
}
