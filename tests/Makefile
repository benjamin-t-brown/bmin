EXE = test_runner
EXE_SUFFIX =

BMIN_DIR = ../src
BMIN_OUTPUT = ../bmin
BMIN_MOD_LIB = $(BMIN_OUTPUT)/lib/libbmin_modules.a
MOD_DIR = $(BMIN_OUTPUT)/modules
MOD_BUILD = modbuild

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

FLAGS = -Wall -std=c++23 -g -fmodules-ts
INCLUDES = -I$(MOD_DIR) -I.
LIBS = -L$(BMIN_OUTPUT)/lib -lbmin_modules

ifeq ($(OS),Windows_NT)
  EXE_SUFFIX = .exe
  LIBS += -mconsole -lmingw32
endif

# BMI producers (object files also end up in libbmin_modules.a; we only need BMIs here).
MOD_IFACE_OBJS = \
  $(MOD_BUILD)/bmin.types.o \
  $(MOD_BUILD)/bmin.detail.o \
  $(MOD_BUILD)/bmin.dynarray.o \
  $(MOD_BUILD)/bmin.unique_ptr.o \
  $(MOD_BUILD)/bmin.string.o \
  $(MOD_BUILD)/bmin.list.o \
  $(MOD_BUILD)/bmin.queue.o \
  $(MOD_BUILD)/bmin.hash.o \
  $(MOD_BUILD)/bmin.map.o \
  $(MOD_BUILD)/bmin.stringstream.o \
  $(MOD_BUILD)/bmin.string_interop.o \
  $(MOD_BUILD)/bmin.containers.o

.PHONY: all test test-compile-fail clean bmin

all: test

bmin:
	$(MAKE) -C $(BMIN_DIR) native

$(BMIN_MOD_LIB): bmin

$(MOD_BUILD):
	@mkdir -p $@

$(MOD_BUILD)/bmin.types.o: $(MOD_DIR)/bmin.types.cppm | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.detail.o: $(MOD_DIR)/bmin.detail.cppm $(MOD_BUILD)/bmin.types.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.dynarray.o: $(MOD_DIR)/bmin.dynarray.cppm $(MOD_BUILD)/bmin.detail.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.unique_ptr.o: $(MOD_DIR)/bmin.unique_ptr.cppm $(MOD_BUILD)/bmin.detail.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.string.o: $(MOD_DIR)/bmin.string.cppm $(MOD_BUILD)/bmin.dynarray.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.list.o: $(MOD_DIR)/bmin.list.cppm $(MOD_BUILD)/bmin.detail.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.queue.o: $(MOD_DIR)/bmin.queue.cppm $(MOD_BUILD)/bmin.dynarray.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.hash.o: $(MOD_DIR)/bmin.hash.cppm $(MOD_BUILD)/bmin.string.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.map.o: $(MOD_DIR)/bmin.map.cppm $(MOD_BUILD)/bmin.hash.o $(MOD_BUILD)/bmin.list.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.stringstream.o: $(MOD_DIR)/bmin.stringstream.cppm $(MOD_BUILD)/bmin.string.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.string_interop.o: $(MOD_DIR)/bmin.string_interop.cppm $(MOD_BUILD)/bmin.string.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(MOD_BUILD)/bmin.containers.o: $(MOD_DIR)/bmin.containers.cppm \
		$(MOD_BUILD)/bmin.map.o $(MOD_BUILD)/bmin.queue.o $(MOD_BUILD)/bmin.unique_ptr.o \
		$(MOD_BUILD)/bmin.stringstream.o | $(MOD_BUILD)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

$(EXE)$(EXE_SUFFIX): $(OBJECTS) $(BMIN_MOD_LIB)
	$(CXX) $(FLAGS) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS)

test: $(EXE)$(EXE_SUFFIX)
	./$(EXE)$(EXE_SUFFIX)
	$(SHELL) check_compile_fail.sh

test-compile-fail:
	$(SHELL) check_compile_fail.sh

%.o: %.cpp $(BMIN_MOD_LIB) $(MOD_IFACE_OBJS)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXE) $(EXE).exe
	rm -rf $(MOD_BUILD) gcm.cache
