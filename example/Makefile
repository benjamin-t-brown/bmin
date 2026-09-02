# Standalone word-count example using bmin headers + std::filesystem.
#
#   make          # build wordcount
#   make run      # build and run on sample.txt
#   ./wordcount path/to/file.txt

CXX ?= g++
CXXFLAGS ?= -Wall -std=c++23 -g

BMIN_ROOT ?= $(abspath ../bmin)
BMIN_INC = $(BMIN_ROOT)/include
BMIN_LIB = $(BMIN_ROOT)/lib/libbmin.a

EXE = wordcount
EXE_SUFFIX =

ifeq ($(OS),Windows_NT)
  EXE_SUFFIX = .exe
  LDLIBS_EXTRA = -mconsole -lmingw32
endif

.PHONY: all run clean ensure-lib
.DEFAULT_GOAL := all

all: $(EXE)$(EXE_SUFFIX)

ensure-lib: $(BMIN_LIB)

$(BMIN_LIB):
	@echo "Building bmin header library at $(BMIN_LIB)"
	$(MAKE) -C ../src native

main.o: main.cpp $(BMIN_LIB)
	$(CXX) $(CXXFLAGS) -I$(BMIN_INC) -c main.cpp -o $@

$(EXE)$(EXE_SUFFIX): main.o $(BMIN_LIB)
	$(CXX) $(CXXFLAGS) -o $@ main.o -L$(BMIN_ROOT)/lib -lbmin $(LDLIBS_EXTRA)

run: $(EXE)$(EXE_SUFFIX)
	./$(EXE)$(EXE_SUFFIX) sample.txt

clean:
	rm -f main.o $(EXE) $(EXE).exe
