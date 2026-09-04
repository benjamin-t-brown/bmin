import bmin.queue;

int queueImportProbe() {
  bmin::Queue<int> values;
  values.push(1);
  return values.front();
}
