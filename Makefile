# Detect project name automatically from directory
PROJECT_NAME := $(notdir $(CURDIR))

# Project structure
SRC_DIR = src
INCLUDE_DIRS = -Iinclude

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Default build mode
MODE ?= release

# Build output directories (mode-specific)
OBJ_DIR = obj/$(MODE)
BIN_DIR = bin/$(MODE)
LIBRARY_NAME = $(BIN_DIR)/lib$(PROJECT_NAME).so  # Change this to shared library .so

# Compiler flags
ifeq ($(MODE),debug)
    CFLAGS = -Wall -Wextra -O0 -g \
             -fsanitize=address,undefined \
             -fsanitize-address-use-after-scope \
             -fanalyzer \
             -fPIC \
             $(INCLUDE_DIRS)
else
    CFLAGS = -Wall -Wextra -O2 -fPIC $(INCLUDE_DIRS)
endif

# Linker flags (for shared library)
LDFLAGS = -shared  # For shared library

# Object files (same layout as sources)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, \
    $(notdir $(SRC_FILES:.c=.o)))

# Ensure top-level bin/ and obj/ exist
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR))

# --- Color Variables ---
YELLOW = \033[1;33m
GREEN = \033[1;32m
RED = \033[1;31m
NC = \033[0m  # No Color (reset)

# Main target: Build shared library
all: $(LIBRARY_NAME)

# Link final shared library
$(LIBRARY_NAME): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LDFLAGS) -o $@
	@if [ $$? -eq 0 ]; then \
		printf "\n$(GREEN)Build completed successfully: $(LIBRARY_NAME)$(NC)\n"; \
		printf "Generating directory structure...\n"; \
		tree -L 2 > directory_structure.txt; \
		printf "Directory structure saved to directory_structure.txt\n"; \
	else \
		printf "\n$(RED)Build failed$(NC)\n"; \
		exit 1; \
	fi

# Compile project source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@printf "\n$(YELLOW)Compiled: $<$(NC)\n"

# --- Short targets ---

# Release build (default)
r:
	@$(MAKE) MODE=release

# Debug build
d:
	@$(MAKE) MODE=debug

# Clean everything
c:
	rm -rf obj bin
	@printf "\n$(RED)Cleaned: obj/ and bin/ directories$(NC)\n"

# Error handling and final status message
.PHONY: all clean d r c
