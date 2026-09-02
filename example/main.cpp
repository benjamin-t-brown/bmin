// Word-count example: bmin headers + std::filesystem.
// Modules twin: ../module_example/main.cpp
//
//   make run
//   ./wordcount sample.txt
//
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "String.h"
#include "DynArray.h"
#include "Map.h"
#include "UniquePtr.h"

namespace fs = std::filesystem;

namespace {

bool isWordChar(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '\'';
}

char toLower(char c) {
  if (c >= 'A' && c <= 'Z') {
    return static_cast<char>(c - 'A' + 'a');
  }
  return c;
}

bmin::String readFile(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    return bmin::String();
  }

  const auto size = fs::file_size(path);
  bmin::String out;
  out.reserve(static_cast<std::size_t>(size));

  char buf[4096];
  while (in.read(buf, sizeof(buf)) || in.gcount() > 0) {
    out.append(buf, static_cast<std::size_t>(in.gcount()));
  }
  return out;
}

struct WordCount {
  bmin::String word;
  int count = 0;
};

// Owns the frequency table and a sorted view for printing.
class Counter {
  bmin::Map<bmin::String, int> _counts;

public:
  void addWord(const bmin::String& word) {
    if (word.empty()) {
      return;
    }
    ++_counts[word];
  }

  void ingest(const bmin::String& text) {
    bmin::String current;
    for (char c : text) {
      if (isWordChar(c)) {
        current.append(toLower(c));
      } else if (!current.empty()) {
        addWord(current);
        current.clear();
      }
    }
    if (!current.empty()) {
      addWord(current);
    }
  }

  bmin::DynArray<WordCount> ranked() const {
    bmin::DynArray<WordCount> rows;
    for (auto it = _counts.begin(); it != _counts.end(); ++it) {
      rows.pushBack(WordCount{(*it).key, (*it).value});
    }

    // Simple insertion sort by count desc, then word asc.
    for (std::size_t i = 1; i < rows.size(); ++i) {
      WordCount key = bmin::move(rows[i]);
      std::size_t j = i;
      while (j > 0) {
        const WordCount& prev = rows[j - 1];
        const bool outOfOrder =
            prev.count < key.count ||
            (prev.count == key.count && prev.word > key.word);
        if (!outOfOrder) {
          break;
        }
        rows[j] = bmin::move(rows[j - 1]);
        --j;
      }
      rows[j] = bmin::move(key);
    }
    return rows;
  }

  std::size_t uniqueWords() const {
    return _counts.size();
  }
};

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::fprintf(stderr, "usage: %s <file>\n", argc > 0 ? argv[0] : "wordcount");
    return 2;
  }

  const fs::path path(argv[1]);
  std::error_code ec;
  if (!fs::is_regular_file(path, ec)) {
    std::fprintf(stderr, "not a readable file: %s\n", path.string().c_str());
    return 1;
  }

  bmin::UniquePtr<Counter> counter(bmin::makeUnique<Counter>());
  counter->ingest(readFile(path));

  const bmin::DynArray<WordCount> rows = counter->ranked();
  std::printf("file: %s\n", path.string().c_str());
  std::printf("unique words: %zu\n", counter->uniqueWords());
  std::printf("%-20s %s\n", "word", "count");
  std::printf("%-20s %s\n", "----", "-----");
  for (std::size_t i = 0; i < rows.size(); ++i) {
    std::printf("%-20s %d\n", rows[i].word.cStr(), rows[i].count);
  }
  return 0;
}
