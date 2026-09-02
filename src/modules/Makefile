# Dual-ship: named modules alongside the header library.
# Header consumers keep using libbmin.a; module consumers use libbmin_modules.a
# + the .cppm sources (rebuild BMIs with their compiler).

CXX ?= g++
AR ?= ar
CXXFLAGS = -Wall -std=c++23 -g -fmodules-ts
INCLUDES = -I.

BUILD = build
BENCH = compile_bench
INSTALL_DIR = ../../bmin
HDR_INC = -I$(BENCH) -I$(INSTALL_DIR)/include
MOD_INC = -I$(BENCH) -I.
LIB_A = $(INSTALL_DIR)/lib/libbmin.a
LIB_MODULES = $(BUILD)/libbmin_modules.a

.PHONY: all modules smoke clean time-compile ensure-lib install dist lib

all: smoke

modules: smoke

$(BUILD) $(BUILD)/headers $(BUILD)/modules:
	@mkdir -p $@

$(BUILD)/bmin.types.o: bmin.types.cppm | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.detail.o: bmin.detail.cppm | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.detail-impl.o: bmin.detail.cpp $(BUILD)/bmin.detail.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.dynarray.o: bmin.dynarray.cppm $(BUILD)/bmin.detail.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.unique_ptr.o: bmin.unique_ptr.cppm $(BUILD)/bmin.detail.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.string.o: bmin.string.cppm $(BUILD)/bmin.dynarray.o $(BUILD)/bmin.detail.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.string-impl.o: bmin.string.cpp $(BUILD)/bmin.string.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.list.o: bmin.list.cppm $(BUILD)/bmin.detail.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.queue.o: bmin.queue.cppm $(BUILD)/bmin.dynarray.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.hash.o: bmin.hash.cppm $(BUILD)/bmin.string.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.map.o: bmin.map.cppm $(BUILD)/bmin.hash.o $(BUILD)/bmin.list.o $(BUILD)/bmin.dynarray.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.stringstream.o: bmin.stringstream.cppm $(BUILD)/bmin.string.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.stringstream-impl.o: bmin.stringstream.cpp $(BUILD)/bmin.stringstream.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.string_interop.o: bmin.string_interop.cppm $(BUILD)/bmin.string.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.string_interop-impl.o: bmin.string_interop.cpp $(BUILD)/bmin.string_interop.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/bmin.containers.o: bmin.containers.cppm \
		$(BUILD)/bmin.string.o $(BUILD)/bmin.unique_ptr.o $(BUILD)/bmin.dynarray.o \
		$(BUILD)/bmin.list.o $(BUILD)/bmin.queue.o $(BUILD)/bmin.hash.o $(BUILD)/bmin.map.o \
		$(BUILD)/bmin.stringstream.o | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

MODULE_OBJS = \
	$(BUILD)/bmin.types.o \
	$(BUILD)/bmin.detail.o \
	$(BUILD)/bmin.detail-impl.o \
	$(BUILD)/bmin.dynarray.o \
	$(BUILD)/bmin.unique_ptr.o \
	$(BUILD)/bmin.string.o \
	$(BUILD)/bmin.string-impl.o \
	$(BUILD)/bmin.list.o \
	$(BUILD)/bmin.queue.o \
	$(BUILD)/bmin.hash.o \
	$(BUILD)/bmin.map.o \
	$(BUILD)/bmin.stringstream.o \
	$(BUILD)/bmin.stringstream-impl.o \
	$(BUILD)/bmin.string_interop.o \
	$(BUILD)/bmin.string_interop-impl.o \
	$(BUILD)/bmin.containers.o

$(LIB_MODULES): $(MODULE_OBJS) | $(BUILD)
	$(AR) rcs $@ $(MODULE_OBJS)

lib: $(LIB_MODULES)

$(BUILD)/smoke.o: smoke.cpp $(MODULE_OBJS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/smoke: $(BUILD)/smoke.o $(LIB_MODULES)
	$(CXX) $(CXXFLAGS) -o $@ $(BUILD)/smoke.o $(LIB_MODULES)

smoke: $(BUILD)/smoke
	./$(BUILD)/smoke

# Headers-only install from parent (avoids recurse into modules).
ensure-lib:
	@$(MAKE) -C .. install-headers

# --- dual-ship install into ../../bmin ---

MODULE_SOURCES = \
	assert.h \
	bmin.types.cppm \
	bmin.detail.cppm \
	bmin.detail.cpp \
	bmin.dynarray.cppm \
	bmin.unique_ptr.cppm \
	bmin.string.cppm \
	bmin.string.cpp \
	bmin.list.cppm \
	bmin.queue.cppm \
	bmin.hash.cppm \
	bmin.map.cppm \
	bmin.stringstream.cppm \
	bmin.stringstream.cpp \
	bmin.string_interop.cppm \
	bmin.string_interop.cpp \
	bmin.containers.cppm

install dist: $(LIB_MODULES)
	@echo "Installing modules into $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR)/lib
	@mkdir -p $(INSTALL_DIR)/modules
	@mkdir -p $(INSTALL_DIR)/modules/make
	cp -f $(LIB_MODULES) $(INSTALL_DIR)/lib/
	cp -f $(MODULE_SOURCES) $(INSTALL_DIR)/modules/
	cp -f make/use.mk make/build-bmi.mk $(INSTALL_DIR)/modules/make/
	cp -f MODULES.md $(INSTALL_DIR)/modules/README.md
	@echo "Installed libbmin_modules.a, module sources, and make/ helpers."

# --- compile-time comparison ---

HDR_WORKERS = worker1 worker2 worker3 worker4 worker5
HDR_OBJS = $(patsubst %,$(BUILD)/headers/%.o,$(HDR_WORKERS)) \
	$(BUILD)/headers/main.o $(BUILD)/headers/bench_workers.o

MOD_WORKERS = worker1 worker2 worker3 worker4 worker5
MOD_OBJS = $(patsubst %,$(BUILD)/modules/%.o,$(MOD_WORKERS)) \
	$(BUILD)/modules/main.o $(BUILD)/modules/bench_workers.o

$(BUILD)/headers/%.o: $(BENCH)/headers/%.cpp | $(BUILD)/headers
	$(CXX) -Wall -std=c++23 -O0 -g $(HDR_INC) -c $< -o $@

$(BUILD)/headers/main.o: $(BENCH)/main.cpp | $(BUILD)/headers
	$(CXX) -Wall -std=c++23 -O0 -g $(HDR_INC) -c $< -o $@

$(BUILD)/headers/bench_workers.o: $(BENCH)/bench_workers.cpp | $(BUILD)/headers
	$(CXX) -Wall -std=c++23 -O0 -g $(HDR_INC) -c $< -o $@

$(BUILD)/modules/%.o: $(BENCH)/modules/%.cpp $(MODULE_OBJS) | $(BUILD)/modules
	$(CXX) $(CXXFLAGS) -O0 $(MOD_INC) -c $< -o $@

$(BUILD)/modules/main.o: $(BENCH)/main.cpp | $(BUILD)/modules
	$(CXX) -Wall -std=c++23 -O0 -g $(MOD_INC) -c $< -o $@

$(BUILD)/modules/bench_workers.o: $(BENCH)/bench_workers.cpp | $(BUILD)/modules
	$(CXX) -Wall -std=c++23 -O0 -g $(MOD_INC) -c $< -o $@

$(BUILD)/bench-headers: $(HDR_OBJS) ensure-lib
	$(CXX) -Wall -std=c++23 -O0 -g -o $@ $(HDR_OBJS) $(LIB_A)

$(BUILD)/bench-modules: $(MOD_OBJS) $(LIB_MODULES)
	$(CXX) $(CXXFLAGS) -O0 -o $@ $(MOD_OBJS) $(LIB_MODULES)

define TIME_CMD
if command -v /usr/bin/time >/dev/null 2>&1; then \
  /usr/bin/time -f 'wall %e sec' $(1); \
else \
  TIMEFORMAT='wall %R sec'; time $(1); \
fi
endef

time-compile: $(LIB_MODULES) ensure-lib
	@echo "=== clean compile: headers (#include) ==="
	@rm -f $(HDR_OBJS) $(BUILD)/bench-headers $(BUILD)/bench-headers.exe
	@$(call TIME_CMD,$(MAKE) --no-print-directory $(BUILD)/bench-headers)
	@echo "=== clean compile: modules (import) ==="
	@rm -f $(MOD_OBJS) $(BUILD)/bench-modules $(BUILD)/bench-modules.exe
	@$(call TIME_CMD,$(MAKE) --no-print-directory $(BUILD)/bench-modules)
	@./$(BUILD)/bench-headers
	@./$(BUILD)/bench-modules
	@echo "both benches ran OK"

clean:
	rm -rf $(BUILD)
	rm -rf gcm.cache
