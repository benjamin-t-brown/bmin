# Reusable GCC rules for compiling bmin module interfaces.
#
# Required before including:
#   BMIN_MODULE_ROOT
#   BMIN_MODULE_OBJDIR
#   BMIN_MODULE_CXXFLAGS
# Optional:
#   BMIN_MODULE_INCLUDES
#   BMIN_MODULE_INTERFACE_FLAGS
#   BMIN_MODULE_EXTRA_PREREQS

include $(dir $(lastword $(MAKEFILE_LIST)))module-graph.mk

BMIN_INTERFACE_OBJECTS := $(patsubst %,$(BMIN_MODULE_OBJDIR)/%.o,$(BMIN_INTERFACE_MODULES))

$(BMIN_MODULE_OBJDIR):
	@mkdir -p $@

define BMIN_NATIVE_INTERFACE_RULE
$(BMIN_MODULE_OBJDIR)/$(1).o: \
		$(BMIN_MODULE_ROOT)/$(1).cppm \
		$(patsubst %,$(BMIN_MODULE_OBJDIR)/%.o,$(BMIN_DEPS_$(1))) \
		$(BMIN_MODULE_EXTRA_PREREQS) | $(BMIN_MODULE_OBJDIR)
	$$(CXX) $$(BMIN_MODULE_CXXFLAGS) $$(BMIN_MODULE_INTERFACE_FLAGS) \
		$$(BMIN_MODULE_INCLUDES) -c $$< -o $$@
endef

$(foreach module,$(BMIN_INTERFACE_MODULES),\
  $(eval $(call BMIN_NATIVE_INTERFACE_RULE,$(module))))
