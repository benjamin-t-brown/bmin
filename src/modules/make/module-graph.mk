# Canonical bmin module graph.

BMIN_COMPONENT_MODULES := \
	bmin.core \
	bmin.dynarray \
	bmin.unique_ptr \
	bmin.string \
	bmin.list \
	bmin.queue \
	bmin.hash \
	bmin.map \
	bmin.stringstream \
	bmin.string_interop

BMIN_PRIMARY_MODULE := bmin.containers
BMIN_INTERFACE_MODULES := $(BMIN_COMPONENT_MODULES) $(BMIN_PRIMARY_MODULE)

BMIN_IMPLEMENTATION_MODULES := \
	bmin.core \
	bmin.string \
	bmin.stringstream \
	bmin.string_interop

BMIN_DEPS_bmin.dynarray := bmin.core
BMIN_DEPS_bmin.unique_ptr := bmin.core
BMIN_DEPS_bmin.string := bmin.core bmin.dynarray
BMIN_DEPS_bmin.list := bmin.core
BMIN_DEPS_bmin.queue := bmin.core
BMIN_DEPS_bmin.hash := bmin.string
BMIN_DEPS_bmin.map := bmin.dynarray bmin.hash bmin.list
BMIN_DEPS_bmin.stringstream := bmin.string
BMIN_DEPS_bmin.string_interop := bmin.string
BMIN_DEPS_bmin.containers := \
	bmin.core bmin.dynarray bmin.list bmin.queue bmin.hash bmin.map \
	bmin.unique_ptr bmin.string bmin.stringstream
