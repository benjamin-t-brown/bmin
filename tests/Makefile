EXE = test_runner
EXE_SUFFIX =

BMIN_DIR = ../src
BMIN_OUTPUT = ../bmin
BMIN_LIB = $(BMIN_OUTPUT)/lib/libbmin.a

TEST_SRCS = \
  test_main.cpp \
  test_string.cpp \
  test_dynarray.cpp \
  test_map.cpp \
  test_uniqueptr.cpp \
  test_stringinterop.cpp \
  test_custom_types.cpp \
  test_queue.cpp \
  test_list.cpp \
  test_stringstream.cpp

OBJECTS = $(TEST_SRCS:.cpp=.o)

FLAGS = -Wall -std=c++20 -g
INCLUDES = -I$(BMIN_OUTPUT)/include -I.
LIBS = -L$(BMIN_OUTPUT)/lib -lbmin

ifeq ($(OS),Windows_NT)
  EXE_SUFFIX = .exe
  LIBS += -mconsole -lmingw32
endif

.PHONY: all test test-compile-fail clean bmin

all: test

bmin:
	$(MAKE) -C $(BMIN_DIR) native

$(BMIN_LIB): bmin

$(EXE)$(EXE_SUFFIX): $(OBJECTS) $(BMIN_LIB)
	$(CXX) $(FLAGS) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS)

test: $(EXE)$(EXE_SUFFIX)
	./$(EXE)$(EXE_SUFFIX)
	$(SHELL) check_compile_fail.sh

test-compile-fail:
	$(SHELL) check_compile_fail.sh

%.o: %.cpp $(BMIN_LIB)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXE) $(EXE).exe
