# Compiler and flags
CC = gcc
BASE_CFLAGS = -Wall -Iinclude
LIBS = -lreadline

# Default debug level
DEBUG_LEVEL ?= 0

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Output executable
TARGET = $(BIN_DIR)/nutshell

# =================================================================
# SEPARATE BUILD TARGETS - Different Build Configurations
# =================================================================

# Default build (same as release)
all: release

# RELEASE BUILD - Optimized, minimal debug
.PHONY: release
release: DEBUG_LEVEL=0
release: CFLAGS = $(BASE_CFLAGS) -O2 -DNDEBUG -DDEFAULT_DEBUG_LEVEL=$(DEBUG_LEVEL)
release: clean-build setup $(TARGET)
	@echo "=== RELEASE BUILD COMPLETE ==="
	@echo "Debug level: $(DEBUG_LEVEL) (disabled)"
	@echo "Optimizations: Enabled (-O2)"
	@echo "Usage: ./$(TARGET)"

# DEBUG BUILD - Full debug info, no optimization
.PHONY: debug
debug: DEBUG_LEVEL=3
debug: CFLAGS = $(BASE_CFLAGS) -g -O0 -DDEBUG -DDEFAULT_DEBUG_LEVEL=$(DEBUG_LEVEL)
debug: clean-build setup $(TARGET)
	@echo "=== DEBUG BUILD COMPLETE ==="
	@echo "Debug level: $(DEBUG_LEVEL) (INFO level)"
	@echo "Optimizations: Disabled (-O0)"
	@echo "Usage: ./$(TARGET) (will show debug output)"

# VERBOSE BUILD - Maximum debug output
.PHONY: verbose
verbose: DEBUG_LEVEL=4
verbose: CFLAGS = $(BASE_CFLAGS) -g -O0 -DDEBUG -DVERBOSE -DDEFAULT_DEBUG_LEVEL=$(DEBUG_LEVEL)
verbose: clean-build setup $(TARGET)
	@echo "=== VERBOSE BUILD COMPLETE ==="
	@echo "Debug level: $(DEBUG_LEVEL) (VERBOSE level)"
	@echo "Optimizations: Disabled (-O0)"
	@echo "Usage: ./$(TARGET) (will show ALL debug output)"

# PROFILE BUILD - For performance analysis
.PHONY: profile
profile: DEBUG_LEVEL=1
profile: CFLAGS = $(BASE_CFLAGS) -g -pg -O1 -DDEFAULT_DEBUG_LEVEL=$(DEBUG_LEVEL)
profile: clean-build setup $(TARGET)
	@echo "=== PROFILE BUILD COMPLETE ==="
	@echo "Debug level: $(DEBUG_LEVEL) (ERROR level only)"
	@echo "Profiling: Enabled (-pg)"
	@echo "Usage: ./$(TARGET) then 'gprof $(TARGET) gmon.out'"

# DEVELOPMENT BUILD - Quick compilation for testing
.PHONY: dev
dev: DEBUG_LEVEL=2
dev: CFLAGS = $(BASE_CFLAGS) -g -O0 -DDEBUG -DDEFAULT_DEBUG_LEVEL=$(DEBUG_LEVEL)
dev: setup $(TARGET)
	@echo "=== DEVELOPMENT BUILD COMPLETE ==="
	@echo "Debug level: $(DEBUG_LEVEL) (WARN level)"
	@echo "Fast compilation: No clean, minimal optimization"

# =================================================================
# BUILD RULES (same for all targets)
# =================================================================

# Linking
$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compiling .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Setup folders if missing
setup:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Clean for fresh build (used by some targets)
clean-build:
	rm -rf $(OBJ_DIR)/*.o

# Full clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# =================================================================
# RUN TARGETS - Run with different configurations
# =================================================================

# Run release build
run-release: release
	@echo "Running release build..."
	./$(TARGET) -q

# # Run debug build
run-debug: debug
	@echo "Running debug build..."
	./$(TARGET) -d 3

# # Run verbose build
run-verbose: verbose
	@echo "Running verbose build..."
	./$(TARGET) -v

# # Run with custom debug level
run-custom: dev
	@echo "Running with custom debug level..."
	./$(TARGET) -d $(DEBUG_LEVEL)



# Legacy run target (uses release)
run: run-release

# =================================================================
# UTILITY TARGETS
# =================================================================

# Show build information
info:
	@echo "=== BUILD CONFIGURATION INFO ==="
	@echo "Available build targets:"
	@echo "  release  - Optimized build (DEBUG_LEVEL=0)"
	@echo "  debug    - Debug build (DEBUG_LEVEL=3)"
	@echo "  verbose  - Verbose debug (DEBUG_LEVEL=4)"
	@echo "  profile  - Profiling build (DEBUG_LEVEL=1)"
	@echo "  dev      - Development build (DEBUG_LEVEL=2)"
	@echo ""
	@echo "Run targets:"
	@echo "  run-release, run-debug, run-verbose, run-custom"
	@echo ""
	@echo "Current settings:"
	@echo "  CC = $(CC)"
	@echo "  DEFAULT DEBUG_LEVEL = $(DEBUG_LEVEL)"

# Help target
help: info
	@echo ""
	@echo "Other targets:"
	@echo "  clean    - Remove all build files"
	@echo "  setup    - Create directories"
	@echo "  help     - Show this help"

# Declare phony targets
.PHONY: all clean run setup help info clean-build
.PHONY: release debug verbose profile dev
.PHONY: run-release run-debug run-verbose run-custom
