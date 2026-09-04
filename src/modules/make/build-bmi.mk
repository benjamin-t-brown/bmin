# Compile bmin module interfaces into ./gcm.cache (run from the consumer dir).
#
#   make -f path/to/bmin/modules/make/build-bmi.mk BMIN_MOD=path/to/bmin/modules

BMIN_MOD ?= .
include $(dir $(lastword $(MAKEFILE_LIST)))config.mk

FLAGS = $(BMIN_MODULE_CXXFLAGS) $(BMIN_MODULE_INTERFACE_FLAGS) -I$(BMIN_MOD)
OBJDIR = .bmin-bmi

.PHONY: all clean

all: \
	$(OBJDIR)/bmin.core.o \
	$(OBJDIR)/bmin.dynarray.o \
	$(OBJDIR)/bmin.unique_ptr.o \
	$(OBJDIR)/bmin.string.o \
	$(OBJDIR)/bmin.list.o \
	$(OBJDIR)/bmin.queue.o \
	$(OBJDIR)/bmin.hash.o \
	$(OBJDIR)/bmin.map.o \
	$(OBJDIR)/bmin.stringstream.o \
	$(OBJDIR)/bmin.string_interop.o \
	$(OBJDIR)/bmin.containers.o
	@mkdir -p gcm.cache
	@touch gcm.cache/.bmin-ready

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/bmin.core.o: $(BMIN_MOD)/bmin.core.cppm | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.dynarray.o: $(BMIN_MOD)/bmin.dynarray.cppm $(OBJDIR)/bmin.core.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.unique_ptr.o: $(BMIN_MOD)/bmin.unique_ptr.cppm $(OBJDIR)/bmin.core.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.string.o: $(BMIN_MOD)/bmin.string.cppm $(OBJDIR)/bmin.dynarray.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.list.o: $(BMIN_MOD)/bmin.list.cppm $(OBJDIR)/bmin.core.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.queue.o: $(BMIN_MOD)/bmin.queue.cppm $(OBJDIR)/bmin.dynarray.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.hash.o: $(BMIN_MOD)/bmin.hash.cppm $(OBJDIR)/bmin.string.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.map.o: $(BMIN_MOD)/bmin.map.cppm $(OBJDIR)/bmin.hash.o $(OBJDIR)/bmin.list.o $(OBJDIR)/bmin.dynarray.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.stringstream.o: $(BMIN_MOD)/bmin.stringstream.cppm $(OBJDIR)/bmin.string.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.string_interop.o: $(BMIN_MOD)/bmin.string_interop.cppm $(OBJDIR)/bmin.string.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJDIR)/bmin.containers.o: $(BMIN_MOD)/bmin.containers.cppm \
		$(OBJDIR)/bmin.map.o $(OBJDIR)/bmin.queue.o $(OBJDIR)/bmin.unique_ptr.o \
		$(OBJDIR)/bmin.stringstream.o | $(OBJDIR)
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) gcm.cache
