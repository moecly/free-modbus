PROJECT := serial-slave

# Set directories
BUILD_DIR := ./build
BIN_DIR := $(BUILD_DIR)/bin
OBJS_DIR := $(BUILD_DIR)/obj
SRCS_DIR := .

# Set var
BIN := $(BIN_DIR)/$(PROJECT)
ifeq ($(OS),Windows_NT)
	BIN := $(BIN).exe
endif

INC := -I ./inc
SRC_EXT := c
OBJ_EXT := o
MKDIR := mkdir
MKDIR_FLAGS := -p
RM := rm
RM_FLAGS := -rf

# Set compiler
CC := aarch64-nuvoton-linux-gnu-gcc
CFLAGS := -Wall -Wextra 
COMPILE = $(CC) $(CFLAGS) $(INC)

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
	@$(CC) -o $(BIN) $^

clean:
	$(RM) $(RM_FLAGS) $(BUILD_DIR)
