# Compile bmin module interfaces into ./gcm.cache (run from the consumer dir).
#
#   make -f path/to/bmin/modules/make/build-bmi.mk BMIN_MOD=path/to/bmin/modules

BMIN_MOD ?= .
include $(dir $(lastword $(MAKEFILE_LIST)))config.mk

OBJDIR = .bmin-bmi
BMIN_CACHE_KEY := $(shell sh $(dir $(lastword $(MAKEFILE_LIST)))cache-key.sh "$(CXX)" "$(subst ",\",$(BMIN_MODULE_CXXFLAGS) $(BMIN_MODULE_INTERFACE_FLAGS))")
BMIN_CACHE_STAMP := gcm.cache/.bmin-config-$(BMIN_CACHE_KEY)
BMIN_READY_STAMP := gcm.cache/.bmin-ready-$(BMIN_CACHE_KEY)

BMIN_MODULE_ROOT := $(BMIN_MOD)
BMIN_MODULE_OBJDIR := $(OBJDIR)
BMIN_MODULE_INCLUDES := -I$(BMIN_MOD)
BMIN_MODULE_EXTRA_PREREQS := $(BMIN_CACHE_STAMP)
.DEFAULT_GOAL := all
include $(dir $(lastword $(MAKEFILE_LIST)))native-rules.mk

.PHONY: all clean

all: $(BMIN_INTERFACE_OBJECTS)
	@touch $(BMIN_READY_STAMP)

$(BMIN_CACHE_STAMP):
	rm -rf gcm.cache $(OBJDIR)
	@mkdir -p gcm.cache
	@touch $@

$(OBJDIR): | $(BMIN_CACHE_STAMP)

clean:
	rm -rf $(OBJDIR) gcm.cache
