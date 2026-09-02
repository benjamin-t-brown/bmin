import bmin.types;
import bmin.detail;
import bmin.containers;

int main() {
  static_assert(sizeof(bmin::Size) >= 4);

  bmin::String s("modules");
  s += " ok";

  bmin::DynArray<int> values;
  values.pushBack(1);
  values.pushBack(2);
  values.pushBack(3);

  bmin::DynArray<bmin::String> parts;
  parts.pushBack(bmin::String("a"));
  parts.pushBack(bmin::String("b"));
  bmin::String joined = bmin::String::join(",", parts);

  bmin::List<int> list;
  list.pushBack(10);
  list.pushFront(5);

  bmin::Queue<int> q;
  q.push(1);
  q.push(2);
  q.pop();

  bmin::Map<bmin::String, int> counts;
  counts.insert(s, static_cast<int>(values.size()));
  counts["extra"] = 7;

  bmin::UniquePtr<bmin::String> heap(new bmin::String("heap"));

  int x = 7;
  int y = bmin::exchange(x, 9);
  (void)y;
  if (x != 9) {
    return 2;
  }

  return (s.size() == 10 && values.size() == 3 && joined == "a,b" &&
          list.size() == 2 && q.size() == 1 && counts.size() == 2 &&
          heap->size() == 4)
             ? 0
             : 1;
}
