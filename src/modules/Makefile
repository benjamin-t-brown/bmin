# Dual-ship: named modules alongside the header library.
# Header consumers keep using libbmin.a; module consumers use libbmin_modules.a
# + the .cppm sources (rebuild BMIs with their compiler).

include make/config.mk

AR ?= ar
INCLUDES = -I.

BUILD = build
BENCH = compile_bench
INSTALL_DIR = ../../bmin
HDR_INC = -I$(BENCH) -I$(INSTALL_DIR)/include
MOD_INC = -I$(BENCH) -I.
LIB_A = $(INSTALL_DIR)/lib/libbmin.a
LIB_MODULES = $(BUILD)/libbmin_modules.a

.PHONY: all check modules smoke public-import-check internal-module-checks \
	validate-module-graph clean time-compile ensure-lib install dist lib

all: smoke

modules: smoke

BMIN_CACHE_KEY := $(shell sh make/cache-key.sh "$(CXX)" "$(subst ",\",$(BMIN_MODULE_CXXFLAGS) $(BMIN_MODULE_INTERFACE_FLAGS))")
BMIN_CACHE_STAMP := gcm.cache/.bmin-config-$(BMIN_CACHE_KEY)

BMIN_MODULE_ROOT := .
BMIN_MODULE_OBJDIR := $(BUILD)
BMIN_MODULE_INCLUDES := $(INCLUDES)
BMIN_MODULE_EXTRA_PREREQS := $(BMIN_CACHE_STAMP)
include make/native-rules.mk

BMIN_IMPLEMENTATION_OBJECTS := $(patsubst %,$(BUILD)/%-impl.o,$(BMIN_IMPLEMENTATION_MODULES))
MODULE_OBJS := $(BMIN_INTERFACE_OBJECTS) $(BMIN_IMPLEMENTATION_OBJECTS)

$(BMIN_CACHE_STAMP):
	rm -rf gcm.cache
	@mkdir -p gcm.cache
	@touch $@

$(BUILD): | $(BMIN_CACHE_STAMP)

$(BUILD)/headers $(BUILD)/modules:
	@mkdir -p $@

$(BUILD)/%-impl.o: %.cpp $(BUILD)/%.o | $(BUILD)
	$(CXX) $(BMIN_MODULE_CXXFLAGS) $(INCLUDES) -c $< -o $@

$(LIB_MODULES): $(MODULE_OBJS) | $(BUILD)
	$(AR) rcs $@ $(MODULE_OBJS)

lib: $(LIB_MODULES)

$(BUILD)/smoke.o: smoke.cpp $(MODULE_OBJS) | $(BUILD)
	$(CXX) $(BMIN_MODULE_CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/smoke: $(BUILD)/smoke.o $(LIB_MODULES)
	$(CXX) $(BMIN_MODULE_CXXFLAGS) -o $@ $(BUILD)/smoke.o $(LIB_MODULES)

smoke: $(BUILD)/smoke
	./$(BUILD)/smoke

public-import-check: smoke
	@echo "public import: OK"

internal-module-checks: $(BMIN_INTERFACE_OBJECTS)
	@echo "internal modules: OK"

check: validate-module-graph public-import-check internal-module-checks

validate-module-graph:
	@set -e; \
	actual='$(sort $(basename $(notdir $(wildcard bmin.*.cppm))))'; \
	expected='$(sort $(BMIN_INTERFACE_MODULES))'; \
	if [ "$$actual" != "$$expected" ]; then \
		echo "bmin interface graph mismatch"; echo "actual:   $$actual"; echo "declared: $$expected"; exit 1; \
	fi; \
	actual='$(sort $(basename $(notdir $(wildcard bmin.*.cpp))))'; \
	expected='$(sort $(BMIN_IMPLEMENTATION_MODULES))'; \
	if [ "$$actual" != "$$expected" ]; then \
		echo "bmin implementation graph mismatch"; echo "actual:   $$actual"; echo "declared: $$expected"; exit 1; \
	fi; \
	$(foreach module,$(BMIN_INTERFACE_MODULES),actual=$$(sed -nE 's/^(export[[:space:]]+)?import[[:space:]]+(bmin\.[[:alnum:]_]+);/\2/p' $(module).cppm | LC_ALL=C sort | awk 'BEGIN { sep="" } { printf "%s%s", sep, $$0; sep=" " } END { print "" }'); expected='$(sort $(BMIN_DEPS_$(module)))'; if [ "$$actual" != "$$expected" ]; then echo "$(module) import mismatch"; echo "actual:   $$actual"; echo "declared: $$expected"; exit 1; fi; ) \
	echo "bmin module graph: OK"

# Headers-only install from parent (avoids recurse into modules).
ensure-lib:
	@$(MAKE) -C .. install-headers

# --- dual-ship install into ../../bmin ---

MODULE_SOURCES = \
	assert.h \
	$(addsuffix .cppm,$(BMIN_INTERFACE_MODULES)) \
	$(addsuffix .cpp,$(BMIN_IMPLEMENTATION_MODULES))

install dist: $(LIB_MODULES)
	@echo "Installing modules into $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR)/lib
	@mkdir -p $(INSTALL_DIR)/modules
	@mkdir -p $(INSTALL_DIR)/modules/make
	cp -f $(LIB_MODULES) $(INSTALL_DIR)/lib/
	cp -f $(MODULE_SOURCES) $(INSTALL_DIR)/modules/
	cp -f make/config.mk make/module-graph.mk make/native-rules.mk make/cache-key.sh make/use.mk make/build-bmi.mk make/build-bmi-clang.mk $(INSTALL_DIR)/modules/make/
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
	$(CXX) $(BMIN_MODULE_CXXFLAGS) -O0 $(MOD_INC) -c $< -o $@

$(BUILD)/modules/main.o: $(BENCH)/main.cpp | $(BUILD)/modules
	$(CXX) -Wall -std=c++23 -O0 -g $(MOD_INC) -c $< -o $@

$(BUILD)/modules/bench_workers.o: $(BENCH)/bench_workers.cpp | $(BUILD)/modules
	$(CXX) -Wall -std=c++23 -O0 -g $(MOD_INC) -c $< -o $@

$(BUILD)/bench-headers: $(HDR_OBJS) ensure-lib
	$(CXX) -Wall -std=c++23 -O0 -g -o $@ $(HDR_OBJS) $(LIB_A)

$(BUILD)/bench-modules: $(MOD_OBJS) $(LIB_MODULES)
	$(CXX) $(BMIN_MODULE_CXXFLAGS) -O0 -o $@ $(MOD_OBJS) $(LIB_MODULES)

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
