PROJECT := serial-slave

# Set directories
BUILD_DIR := ./build
BIN_DIR := $(BUILD_DIR)/bin
OBJS_DIR := $(BUILD_DIR)/obj
SRCS_DIR := .
DYNAMIC_LIB_DIR := $(BUILD_DIR)/lib

# Set var
BIN := $(BIN_DIR)/$(PROJECT)
ifeq ($(OS),Windows_NT)
	BIN := $(BIN).exe
endif

INC := -I ./inc
SRC_EXT := c
OBJ_EXT := o
DYNAMIC_LIB_EXT := so
MKDIR := mkdir
MKDIR_FLAGS := -p
RM := rm
RM_FLAGS := -rf
LN := ln
LN_FLAGS := -sf

# Set compiler
CC := aarch64-nuvoton-linux-gnu-gcc
CFLAGS := -Wall -Wextra -fPIC
COMPILE = $(CC) $(CFLAGS) $(INC)
DYNAMIC_LIB_NAME := lib$(PROJECT)
DYNAMIC_LIB_VERSION := 1.0
DYNAMIC_LIB := $(DYNAMIC_LIB_DIR)/$(DYNAMIC_LIB_NAME).$(DYNAMIC_LIB_EXT)
DYNAMIC_LIB_FLAGS := -shared

# Recursive search
define find_files
	$(wildcard $(1)/$(2)) $(foreach dir,$(wildcard $(1)/*),$(call find_files,$(dir),$(2)))
endef

# Automatically include all source files
#SRCS := $(shell find $(SRCS_DIR) -type f -name '*.c' -not -path '*/\.*')
SRCS := $(call find_files,$(SRCS_DIR),*.$(SRC_EXT))
OBJECTS := $(patsubst $(SRCS_DIR)/%,$(OBJS_DIR)/%,$(SRCS:%.$(SRC_EXT)=%.$(OBJ_EXT)))

all: default 

$(OBJS_DIR)/%.$(OBJ_EXT): $(SRCS_DIR)/%.$(SRC_EXT)
	@echo "Building project obj file: $<"
	@$(MKDIR) $(MKDIR_FLAGS) $(dir $@)
	@$(COMPILE) -c -o $@ $<

default: $(OBJECTS)
	@echo "Building project bin file: $(BIN)"
	@$(MKDIR) $(MKDIR_FLAGS) $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $(BIN) $^

dylib: compile_source

compile_source: $(OBJECTS)
	@echo "Generate project dynamic lib file: $(DYNAMIC_LIB).$(DYNAMIC_LIB_VERSION)"
	@$(MKDIR) $(MKDIR_FLAGS) $(DYNAMIC_LIB_DIR)
	@$(CC) $(CFLAGS) $(DYNAMIC_LIB_FLAGS) -o $(DYNAMIC_LIB).$(DYNAMIC_LIB_VERSION) $^
	@$(LN) $(LN_FLAGS) $(DYNAMIC_LIB_NAME).$(DYNAMIC_LIB_EXT).$(DYNAMIC_LIB_VERSION) $(DYNAMIC_LIB) 

clean:
	$(RM) $(RM_FLAGS) $(BUILD_DIR)
