#include "TestHarness.h"  // IWYU pragma: keep

#include "DynArray.h"  // IWYU pragma: keep
#include "Map.h"  // IWYU pragma: keep
#include "UniquePtr.h"  // IWYU pragma: keep

struct Point {
  int x;
  int y;

  bool operator==(const Point& o) const {
    return x == o.x && y == o.y;
  }
};

struct Widget {
  int id;

  bool operator==(const Widget& o) const {
    return id == o.id;
  }
};

SUITE(test_custom_types) {
  bmin::DynArray<Point> points;
  points.push_back(Point{1, 2});
  points.push_back(Point{3, 4});
  CHECK_EQ(points.size(), 2u);
  CHECK_EQ(points[1].x, 3);

  bmin::Map<Point, int> pointMap;
  CHECK(pointMap.insert(Point{10, 20}, 99));
  CHECK_EQ((pointMap[Point{10, 20}]), 99);
  CHECK(pointMap.contains(Point{10, 20}));

  bmin::UniquePtr<Widget> w(bmin::makeUnique<Widget>(Widget{7}));
  CHECK(static_cast<bool>(w));
  CHECK_EQ(w->id, 7);

  bmin::DynArray<bmin::UniquePtr<Widget>> widgets;
  widgets.push_back(bmin::makeUnique<Widget>(Widget{1}));
  widgets.push_back(bmin::makeUnique<Widget>(Widget{2}));
  CHECK_EQ(widgets.size(), 2u);
  CHECK_EQ(widgets[0]->id, 1);

  bmin::DynArray<bmin::UniquePtr<Widget>> moved(bmin::move(widgets));
  CHECK_EQ(moved.size(), 2u);
  CHECK(widgets.empty());
}
