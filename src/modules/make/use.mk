# Consumer Make helpers for bmin modules.
#
#   include path/to/bmin/modules/make/use.mk
#
# Provides:
#   BMIN_CXXFLAGS   compile flags (-fmodules-ts, -I …)
#   BMIN_LDLIBS     link flags (-lbmin_modules)
#   bmin-bmi        builds BMIs into ./gcm.cache (via build-bmi.mk)
#
# Example:
#   include ../bmin/modules/make/use.mk
#   main.o: main.cpp bmin-bmi
#           $(CXX) $(BMIN_CXXFLAGS) -c main.cpp -o $@
#   app: main.o
#           $(CXX) $(BMIN_CXXFLAGS) -o $@ main.o $(BMIN_LDLIBS)
#
# libbmin_modules.a is for linking. BMIs are compiler-local (gcm.cache).

_BMIN_MAKE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
include $(_BMIN_MAKE_DIR)/config.mk

# .cppm sources live in the parent modules/ directory.
BMIN_MODULES_DIR := $(abspath $(_BMIN_MAKE_DIR)/..)

# Library install root: prefer sibling ../lib (installed), else repo bmin/.
ifeq ($(wildcard $(BMIN_MODULES_DIR)/../lib/libbmin_modules.a),)
  BMIN_ROOT ?= $(abspath $(BMIN_MODULES_DIR)/../../bmin)
  BMIN_SRC_DIR ?= $(abspath $(BMIN_MODULES_DIR)/..)
else
  BMIN_ROOT ?= $(abspath $(BMIN_MODULES_DIR)/..)
  BMIN_SRC_DIR ?= $(abspath $(BMIN_MODULES_DIR)/../src)
endif

BMIN_CXXFLAGS ?= $(BMIN_MODULE_CXXFLAGS) -I$(BMIN_MODULES_DIR)
BMIN_LDLIBS ?= -L$(BMIN_ROOT)/lib -lbmin_modules

ifeq ($(OS),Windows_NT)
  BMIN_LDLIBS += -mconsole -lmingw32
endif

BMIN_LIB ?= $(BMIN_ROOT)/lib/libbmin_modules.a
BMIN_BMI_STAMP ?= gcm.cache/.bmin-ready

# Included makefiles often appear before the consumer's `all:` rule; without
# this, the first helper target becomes `make`'s default goal.
.DEFAULT_GOAL ?= all

.PHONY: bmin-bmi bmin-ensure-lib

bmin-ensure-lib: $(BMIN_LIB)

$(BMIN_LIB):
	@echo "Building bmin modules library at $(BMIN_LIB)"
	$(MAKE) -C $(BMIN_SRC_DIR) native \
		CXX="$(CXX)" \
		BMIN_MODULE_CXXFLAGS="$(BMIN_MODULE_CXXFLAGS)" \
		BMIN_MODULE_INTERFACE_FLAGS="$(BMIN_MODULE_INTERFACE_FLAGS)"

bmin-bmi: $(BMIN_BMI_STAMP)

$(BMIN_BMI_STAMP): $(BMIN_LIB)
	$(MAKE) -f $(_BMIN_MAKE_DIR)/build-bmi.mk \
		BMIN_MOD=$(BMIN_MODULES_DIR) \
		CXX="$(CXX)" \
		BMIN_MODULE_CXXFLAGS="$(BMIN_MODULE_CXXFLAGS)" \
		BMIN_MODULE_INTERFACE_FLAGS="$(BMIN_MODULE_INTERFACE_FLAGS)"
	@touch $(BMIN_BMI_STAMP)
