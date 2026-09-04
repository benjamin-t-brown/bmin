# Build bmin Clang PCMs and matching objects in the consumer directory.
# The caller supplies flags appropriate for clang++ or em++.

BMIN_MOD ?= .
BMIN_PCMDIR ?= pcm.cache
BMIN_OBJDIR ?= .bmin-bmi
BMIN_CLANG_CXXFLAGS ?= -Wall -std=c++23 -fprebuilt-module-path=$(BMIN_PCMDIR) -I$(BMIN_MOD)
BMIN_CLANG_INTERFACE_FLAGS ?=

_BMIN_CLANG_MAKE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
include $(_BMIN_CLANG_MAKE_DIR)/module-graph.mk

BMIN_CACHE_KEY := $(shell sh $(_BMIN_CLANG_MAKE_DIR)/cache-key.sh "$(CXX)" "$(subst ",\",$(BMIN_CLANG_CXXFLAGS) $(BMIN_CLANG_INTERFACE_FLAGS))")
BMIN_CACHE_STAMP := $(BMIN_PCMDIR)/.bmin-config-$(BMIN_CACHE_KEY)
BMIN_READY_STAMP := $(BMIN_PCMDIR)/.bmin-ready-$(BMIN_CACHE_KEY)
BMIN_PCMS := $(patsubst %,$(BMIN_PCMDIR)/%.pcm,$(BMIN_INTERFACE_MODULES))
BMIN_INTERFACE_OBJECTS := $(patsubst %,$(BMIN_OBJDIR)/%.o,$(BMIN_INTERFACE_MODULES))
BMIN_IMPLEMENTATION_OBJECTS := $(patsubst %,$(BMIN_OBJDIR)/%-impl.o,$(BMIN_IMPLEMENTATION_MODULES))
BMIN_MODULE_OBJECTS := $(BMIN_INTERFACE_OBJECTS) $(BMIN_IMPLEMENTATION_OBJECTS)

.DEFAULT_GOAL := all
.PHONY: all clean

all: $(BMIN_MODULE_OBJECTS)
	@touch $(BMIN_READY_STAMP)

$(BMIN_CACHE_STAMP):
	rm -rf $(BMIN_PCMDIR) $(BMIN_OBJDIR)
	@mkdir -p $(BMIN_PCMDIR)
	@touch $@

$(BMIN_OBJDIR):
	@mkdir -p $@

define BMIN_CLANG_PCM_RULE
$(BMIN_PCMDIR)/$(1).pcm: \
		$(BMIN_MOD)/$(1).cppm \
		$(patsubst %,$(BMIN_PCMDIR)/%.pcm,$(BMIN_DEPS_$(1))) \
		$(BMIN_CACHE_STAMP)
	$$(CXX) $$(BMIN_CLANG_CXXFLAGS) $$(BMIN_CLANG_INTERFACE_FLAGS) \
		--precompile $$< -o $$@
endef

$(foreach module,$(BMIN_INTERFACE_MODULES),\
  $(eval $(call BMIN_CLANG_PCM_RULE,$(module))))

$(BMIN_OBJDIR)/%.o: $(BMIN_PCMDIR)/%.pcm | $(BMIN_OBJDIR)
	$(CXX) $(BMIN_CLANG_CXXFLAGS) -c $< -o $@

$(BMIN_OBJDIR)/%-impl.o: $(BMIN_MOD)/%.cpp $(BMIN_PCMDIR)/%.pcm | $(BMIN_OBJDIR)
	$(CXX) $(BMIN_CLANG_CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BMIN_PCMDIR) $(BMIN_OBJDIR)
