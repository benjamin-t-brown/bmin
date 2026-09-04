# Shared compiler settings for bmin named modules.
#
# GCC does not consistently infer C++ from the .cppm suffix, so module
# interface rules always pass BMIN_MODULE_INTERFACE_FLAGS explicitly.

BMIN_HOST_OS := $(shell uname -s 2>/dev/null)

# On macOS, /usr/bin/g++ is Apple Clang. Prefer the supported GNU compiler
# when the consumer did not choose CXX explicitly.
ifeq ($(BMIN_HOST_OS),Darwin)
  ifeq ($(origin CXX),default)
    CXX := g++-15
  endif
endif

BMIN_MODULE_CXXFLAGS ?= -Wall -std=c++23 -g -fmodules-ts
BMIN_MODULE_INTERFACE_FLAGS ?= -x c++
