# Standalone word-count example using bmin modules + std::filesystem.
#
#   make          # build wordcount
#   make run      # build and run on sample.txt
#   ./wordcount path/to/file.txt

EXE = wordcount
EXE_SUFFIX =

ifeq ($(OS),Windows_NT)
  EXE_SUFFIX = .exe
endif

.PHONY: all run clean
.DEFAULT_GOAL := all
all: $(EXE)$(EXE_SUFFIX)

# Consumer helpers live under modules/make/ (not mixed with .cppm sources).
ifeq ($(wildcard ../bmin/modules/make/use.mk),)
  include ../src/modules/make/use.mk
else
  include ../bmin/modules/make/use.mk
endif

main.o: main.cpp | bmin-bmi
	$(CXX) $(BMIN_CXXFLAGS) -c main.cpp -o $@

$(EXE)$(EXE_SUFFIX): main.o $(BMIN_LIB)
	$(CXX) $(BMIN_CXXFLAGS) -o $@ main.o $(BMIN_LDLIBS)

run: $(EXE)$(EXE_SUFFIX)
	./$(EXE)$(EXE_SUFFIX) sample.txt

clean:
	rm -f main.o $(EXE) $(EXE).exe
	rm -rf .bmin-bmi gcm.cache
