#include "TestHarness.h"  // IWYU pragma: keep
#include "All.h"  // IWYU pragma: keep

SUITE(test_string) {
  bmin::String a("hello");
  a.append(" world");
  CHECK_EQ(a, "hello world");

  bmin::String mid = a.slice(6, 11);
  CHECK_EQ(mid, "world");

  bmin::String tail = a.slice(6);
  CHECK_EQ(tail, "world");

  CHECK_EQ(a.slice(), "hello world");
  CHECK_EQ(a.sliceView(), "hello world");
  CHECK_EQ(a.slice(-5), "world");
  CHECK_EQ(a.sliceView(-5), "world");
  CHECK_EQ(a.slice(0, -1), "hello worl");
  CHECK_EQ(a.sliceView(0, -1), "hello worl");

  CHECK_EQ(a.sliceView(6, 11), "world");
  CHECK_EQ(a.sliceView(6), "world");
  CHECK(a.sliceView(100, 200).empty());

  bmin::String empty = a.slice(100, 200);
  CHECK(empty.empty());

  bmin::DynArray<bmin::String> parts;
  parts.pushBack(bmin::String("a"));
  parts.pushBack(bmin::String("b"));
  parts.pushBack(bmin::String("c"));
  bmin::String joined = bmin::String::join("-", parts);
  CHECK_EQ(joined, "a-b-c");

  bmin::DynArray<bmin::String> arr;
  arr.pushBack(bmin::String("x"));
  arr.pushBack(bmin::String("y"));
  bmin::String joined2 = bmin::String::join(",", arr);
  CHECK_EQ(joined2, "x,y");

  bmin::DynArray<bmin::String> split1 = bmin::String("a-b-c").split("-");
  CHECK_EQ(split1.size(), 3u);
  CHECK_EQ(split1[0], "a");
  CHECK_EQ(split1[1], "b");
  CHECK_EQ(split1[2], "c");
  CHECK_EQ(bmin::String::join("-", split1), "a-b-c");

  bmin::DynArray<bmin::String> split2 = bmin::String("a--b").split("-");
  CHECK_EQ(split2.size(), 3u);
  CHECK_EQ(split2[0], "a");
  CHECK_EQ(split2[1], "");
  CHECK_EQ(split2[2], "b");

  bmin::DynArray<bmin::String> split3 = bmin::String("").split(",");
  CHECK_EQ(split3.size(), 1u);
  CHECK(split3[0].empty());

  bmin::String moved = bmin::move(a);
  CHECK(moved.size() > 0);
  CHECK(a.empty());

  bmin::String left("foo");
  bmin::String right("bar");
  CHECK_EQ(left + right, "foobar");
  CHECK_EQ(left + "baz", "foobaz");
  CHECK_EQ("pre-" + right, "pre-bar");

  bmin::String chained = left + "-" + right + "!";
  CHECK_EQ(chained, "foo-bar!");

  bmin::String acc = bmin::String("a");
  acc += "b";
  acc += bmin::String("c");
  CHECK_EQ(acc, "abc");

  bmin::String sum = bmin::String("hello") + " " + bmin::String("world");
  CHECK_EQ(sum, "hello world");

  CHECK_EQ(bmin::String::fromInt(42), "42");
  CHECK_EQ(bmin::String::fromInt(-7), "-7");
  CHECK_EQ(bmin::String::fromInt(0), "0");

  int n = 0;
  CHECK(bmin::String("123").parseInt(n));
  CHECK_EQ(n, 123);
  CHECK(bmin::String("-99").parseInt(n));
  CHECK_EQ(n, -99);
  CHECK(!bmin::String("12a3").parseInt(n));
  CHECK(!bmin::String("").parseInt(n));

  CHECK_EQ(bmin::toString(42), "42");
  CHECK_EQ(bmin::toString(-7), "-7");
  CHECK_EQ(bmin::toString(1000000000u), "1000000000");
  CHECK_EQ(bmin::toString(2.5), "2.500000");

  bmin::String fromLen("hello", 3);
  CHECK_EQ(fromLen, "hel");
  CHECK_EQ(fromLen.length(), 3u);

  bmin::String copied(fromLen);
  CHECK_EQ(copied, "hel");
  fromLen.append('!');
  CHECK_EQ(copied, "hel");

  bmin::String s("abcdef");
  CHECK_EQ(s[0], 'a');
  CHECK_EQ(s.at(5), 'f');
  s[1] = 'B';
  CHECK_EQ(s, "aBcdef");

  s.assign("xy");
  CHECK_EQ(s, "xy");
  s.assign("12345", 3);
  CHECK_EQ(s, "123");
  s.assign(bmin::String("ok"));
  CHECK_EQ(s, "ok");

  s = bmin::String("hello");
  s.pushBack('!');
  CHECK_EQ(s, "hello!");
  CHECK_EQ(s + '!', "hello!!");
  CHECK_EQ('!' + s, "!hello!");

  s.insert(5, " there");
  CHECK_EQ(s, "hello there!");
  s.insert(0, "say: ");
  CHECK_EQ(s, "say: hello there!");

  s = bmin::String("abcdef");
  s.erase(2, 2);
  CHECK_EQ(s, "abef");
  s.erase(1);
  CHECK_EQ(s, "a");

  CHECK_EQ(s.substr(0, 1), "a");
  CHECK_EQ(bmin::String("hello").substr(1), "ello");
  CHECK(bmin::String("hi").substr(10).empty());

  bmin::String hay("one two three");
  CHECK_EQ(hay.find("two"), 4u);
  CHECK_EQ(hay.find("four"), bmin::String::npos);
  CHECK(hay.contains("three"));
  CHECK(hay.startsWith("one"));
  CHECK(hay.endsWith("three"));
  CHECK(!hay.startsWith("two"));

  CHECK(bmin::String("abc") < bmin::String("abd"));
  CHECK(bmin::String("abc") < "abd");
  CHECK(bmin::String("abd") > "abc");
  CHECK(bmin::String("abc") <= bmin::String("abc"));
  CHECK(bmin::String("abc") >= "abc");

  s = bmin::String("padding");
  s.reserve(64);
  CHECK(s.capacity() >= 64u);
  s.shrinkToFit();
  CHECK_EQ(s.capacity(), s.size());
}
