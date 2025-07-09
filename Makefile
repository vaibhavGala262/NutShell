# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Output executable
TARGET = $(BIN_DIR)/nutshell

# Default rule to build everything
all: setup $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Setup folders if missing
setup:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Clean object and binary files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the shell
run: all
	./$(TARGET)
