CODE=\
lib/String.cpp\
lib/StringStream.cpp\
lib/StringInterop.cpp\
lib/Instantiations.cpp\
lib/internal/List.cpp\
lib/internal/Policy.cpp

TARGET ?= native
BASE_BUILD_DIR = build

ifeq ($(V),1)
  Q =
else
  Q = @
endif

DEBUG=true
ifeq ($(TARGET),wasm)
    # -iquote lib: quoted includes only. -Ilib on Windows shadows <string.h> via lib/String.h.
    INCLUDES= -I. -iquote lib
    FLAGS = -Wall -std=c++23 -Oz -flto
    LIBS =
    CXX = em++
    AR = emar
else
    INCLUDES += -I. -iquote lib
    FLAGS += -Wall -std=c++23
    AR ?= ar
    ifeq ($(OS),Windows_NT)
        LIBS += -mconsole -lmingw32
    else
        UNAME_S := $(shell uname -s)
        ifeq ($(UNAME_S),Darwin)
            LIBS += -L/opt/homebrew/lib -L/usr/local/lib
            INCLUDES += -I/opt/homebrew/include -I/usr/local/include
        else
            LIBS +=
        endif
    endif
    ifeq ($(DEBUG),true)
        FLAGS += -g
    endif
endif
TARGET_SUFFIX = $(TARGET)

OBJ_OUTPUT_DIR = $(BASE_BUILD_DIR)/obj
LIB_OUTPUT_DIR = $(BASE_BUILD_DIR)/lib
BIN_OUTPUT_DIR = $(BASE_BUILD_DIR)/bin
INSTALL_DIR = ../bmin

DIRS_TO_CREATE = $(OBJ_OUTPUT_DIR) $(LIB_OUTPUT_DIR) $(BIN_OUTPUT_DIR)

OBJECTS = $(patsubst %.cpp,$(OBJ_OUTPUT_DIR)/%.o,$(CODE))
DEPENDS = $(patsubst %.cpp,$(OBJ_OUTPUT_DIR)/%.d,$(CODE))

LIB_BMIN = $(LIB_OUTPUT_DIR)/libbmin.a

HEADER_SRC_DIR = lib

.PHONY: native wasm clean test dist $(DIRS_TO_CREATE)

native:
	@$(MAKE) all TARGET=native

wasm:
	@$(MAKE) all TARGET=wasm

files:
	@echo $(CODE)

all: dist

test:
	@$(MAKE) -C ../tests test

$(DIRS_TO_CREATE):
	@mkdir -p $@

$(LIB_BMIN): $(OBJECTS) | $(LIB_OUTPUT_DIR)
	$(AR) rcs $@ $(OBJECTS)

dist: $(LIB_BMIN)
	@echo "Creating library and headers for $(TARGET) at $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR)/lib
	@mkdir -p $(INSTALL_DIR)/include
	@cp $(LIB_BMIN) $(INSTALL_DIR)/lib
	@echo "Copying header files from $(HEADER_SRC_DIR) to $(INSTALL_DIR)/include"
	cp -f $(HEADER_SRC_DIR)/*.h $(INSTALL_DIR)/include
	cp -f $(HEADER_SRC_DIR)/*.hpp $(INSTALL_DIR)/include
	@mkdir -p $(INSTALL_DIR)/include/internal
	cp -f $(HEADER_SRC_DIR)/internal/*.h $(INSTALL_DIR)/include/internal
	@echo "Created $(TARGET) bmin folder at top level directory."

-include $(DEPENDS)

$(OBJ_OUTPUT_DIR)/%.o: %.cpp | $(DIRS_TO_CREATE)
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

clean:
	@echo "Cleaning for all targets..."
	rm -rf $(BASE_BUILD_DIR)
	rm -f libbmin.a
	rm -rf ../bmin
	rm -rf main.d
