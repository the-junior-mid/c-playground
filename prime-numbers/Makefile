# Compiler settings
CC = cl.exe
CFLAGS = /Iinclude /W4 /MD /Zi /c\
LINKER = link.exe

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

# Files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/prime.c
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/prime.o
TARGET = $(BIN_DIR)/is-prime.exe

# Default build rule
all: setup $(TARGET)

# Link object files to create the final Windows executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compile C source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create the obj and data folders if they do not exist
setup:
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# Clean rule to wipe out built files using Windows commands
clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(TARGET) del /q $(TARGET)
	@echo Cleanup complete!