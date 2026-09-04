BMIN_SRC_DIR := ../src
BMIN_OUTPUT := ../bmin
BMIN_INCLUDE := $(BMIN_OUTPUT)/include
BMIN_MODULES := $(BMIN_OUTPUT)/modules
BMIN_HEADER_LIB := $(BMIN_OUTPUT)/lib/libbmin.a
BMIN_MODULE_LIB := $(BMIN_OUTPUT)/lib/libbmin_modules.a
BMIN_DIST_STAMP := $(BMIN_OUTPUT)/.test-artifacts-ready
BMIN_BUILD_INPUTS := \
	$(wildcard $(BMIN_SRC_DIR)/lib/*.cpp) \
	$(wildcard $(BMIN_SRC_DIR)/lib/*.h) \
	$(wildcard $(BMIN_SRC_DIR)/lib/internal/*) \
	$(wildcard $(BMIN_SRC_DIR)/modules/*.cppm) \
	$(wildcard $(BMIN_SRC_DIR)/modules/*.cpp) \
	$(wildcard $(BMIN_SRC_DIR)/modules/make/*)

include $(BMIN_SRC_DIR)/modules/make/config.mk
include $(BMIN_SRC_DIR)/modules/make/module-graph.mk

BUILD := build
HEADER_BUILD := $(BUILD)/headers
MODULE_BUILD := $(BUILD)/modules
DIRECT_BUILD := $(BUILD)/direct-imports

TEST_NAMES := \
  test_main \
  test_string \
  test_dynarray \
  test_map \
  test_uniqueptr \
  test_stringinterop \
  test_custom_types \
  test_queue \
  test_list \
  test_stringstream

HEADER_OBJECTS := $(patsubst %,$(HEADER_BUILD)/%.o,$(TEST_NAMES))
MODULE_OBJECTS := $(patsubst %,$(MODULE_BUILD)/%.o,$(TEST_NAMES))
PUBLIC_IMPORT_SOURCES := direct_import/containers.cpp direct_import/string_interop.cpp
INTERNAL_IMPORT_SOURCES := $(filter-out $(PUBLIC_IMPORT_SOURCES),$(wildcard direct_import/*.cpp))
PUBLIC_IMPORT_OBJECTS := $(patsubst direct_import/%.cpp,$(DIRECT_BUILD)/%.o,$(PUBLIC_IMPORT_SOURCES))
INTERNAL_IMPORT_OBJECTS := $(patsubst direct_import/%.cpp,$(DIRECT_BUILD)/%.o,$(INTERNAL_IMPORT_SOURCES))

HEADER_RUNNER := $(HEADER_BUILD)/test_runner
MODULE_RUNNER := $(MODULE_BUILD)/test_runner
BMIN_CACHE_KEY := $(shell sh $(BMIN_SRC_DIR)/modules/make/cache-key.sh "$(CXX)" "$(subst ",\",$(BMIN_MODULE_CXXFLAGS) $(BMIN_MODULE_INTERFACE_FLAGS))")
MODULE_BMI_STAMP := gcm.cache/.bmin-ready-$(BMIN_CACHE_KEY)

HEADER_FLAGS := -Wall -std=c++23 -g -iquote $(BMIN_INCLUDE) -iquote .
MODULE_FLAGS := $(BMIN_MODULE_CXXFLAGS) -I$(BMIN_MODULES) -I.

.DEFAULT_GOAL := test
.PHONY: test test-headers test-modules public-import-check internal-module-checks direct-imports \
  test-compile-fail test-compile-fail-headers test-compile-fail-modules \
  bmin clean

test: test-headers test-modules public-import-check internal-module-checks test-compile-fail

bmin: $(BMIN_DIST_STAMP)

$(BMIN_DIST_STAMP): $(BMIN_BUILD_INPUTS)
	$(MAKE) -C $(BMIN_SRC_DIR) native \
		CXX="$(CXX)" \
		BMIN_MODULE_CXXFLAGS="$(BMIN_MODULE_CXXFLAGS)" \
		BMIN_MODULE_INTERFACE_FLAGS="$(BMIN_MODULE_INTERFACE_FLAGS)"
	@touch $@

$(BMIN_HEADER_LIB) $(BMIN_MODULE_LIB): $(BMIN_DIST_STAMP)
	@test -f $@

$(MODULE_BMI_STAMP): $(BMIN_MODULE_LIB)
	$(MAKE) -f $(BMIN_MODULES)/make/build-bmi.mk \
		BMIN_MOD=$(abspath $(BMIN_MODULES)) \
		CXX="$(CXX)" \
		BMIN_MODULE_CXXFLAGS="$(BMIN_MODULE_CXXFLAGS)" \
		BMIN_MODULE_INTERFACE_FLAGS="$(BMIN_MODULE_INTERFACE_FLAGS)"

$(HEADER_BUILD) $(MODULE_BUILD) $(DIRECT_BUILD):
	@mkdir -p $@

$(HEADER_BUILD)/%.o: %.cpp $(BMIN_HEADER_LIB) | $(HEADER_BUILD)
	$(CXX) $(HEADER_FLAGS) -c $< -o $@

$(MODULE_BUILD)/%.o: %.cpp $(MODULE_BMI_STAMP) | $(MODULE_BUILD)
	$(CXX) $(MODULE_FLAGS) -DBMIN_TEST_MODULES=1 -c $< -o $@

$(HEADER_RUNNER): $(HEADER_OBJECTS) $(BMIN_HEADER_LIB)
	$(CXX) $(HEADER_FLAGS) -o $@ $(HEADER_OBJECTS) \
		-L$(BMIN_OUTPUT)/lib -lbmin

$(MODULE_RUNNER): $(MODULE_OBJECTS) $(BMIN_MODULE_LIB)
	$(CXX) $(MODULE_FLAGS) -o $@ $(MODULE_OBJECTS) \
		-L$(BMIN_OUTPUT)/lib -lbmin_modules

test-headers: $(HEADER_RUNNER)
	./$(HEADER_RUNNER)

test-modules: $(MODULE_RUNNER)
	./$(MODULE_RUNNER)

$(DIRECT_BUILD)/%.o: direct_import/%.cpp $(MODULE_BMI_STAMP) | $(DIRECT_BUILD)
	$(CXX) $(MODULE_FLAGS) -c $< -o $@

public-import-check: $(PUBLIC_IMPORT_OBJECTS)
	@echo "public imports: OK"

internal-module-checks: $(INTERNAL_IMPORT_OBJECTS)
	@echo "internal modules: OK"

direct-imports: public-import-check internal-module-checks

test-compile-fail: test-compile-fail-headers test-compile-fail-modules

test-compile-fail-headers: $(BMIN_HEADER_LIB)
	BMIN_TEST_CXXFLAGS="-std=c++23 -iquote $(abspath $(BMIN_INCLUDE))" \
		$(SHELL) check_compile_fail.sh headers "$(CXX)"

test-compile-fail-modules: $(MODULE_BMI_STAMP)
	BMIN_TEST_CXXFLAGS="$(MODULE_FLAGS) -DBMIN_TEST_MODULES=1" \
		$(SHELL) check_compile_fail.sh modules "$(CXX)"

clean:
	rm -rf $(BUILD) .bmin-bmi gcm.cache
	rm -f test_*.o test_runner test_runner.exe
