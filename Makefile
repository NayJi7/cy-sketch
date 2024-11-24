# Name of the final executable
EXEC = files.exe/main.exe

# Directory for object files and executable
OBJ_DIR_O = files.o
OBJ_DIR_EXE = files.exe

# Directory for shared libraries
LIB_DIR = files.lib

# List of source files
SRC = draw/main.c draw/form.c

# List of object files (replace .c with .o and add the directory path)
OBJ = $(addprefix $(OBJ_DIR_O)/, $(notdir $(SRC:.c=.o)))

# Compilation command
CC = gcc

# Compilation options (SDL2 and SDL2_gfx libraries included)
CFLAGS = -Wall -Wextra -lSDL2 -lSDL2_gfx -lm

# Rule to create a shared library from form.c
LIBRARY = $(LIB_DIR)/libform.so

# Default rule to generate the executable
all: $(EXEC) $(LIBRARY)

# Link object files to create the executable
$(EXEC): $(OBJ)
	mkdir -p $(OBJ_DIR_EXE)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

# Rule to create the shared library
$(LIBRARY): draw/form.c
	mkdir -p $(LIB_DIR)  # Create library directory if it does not exist
	$(CC) -shared -fPIC -o $(LIBRARY) draw/form.c $(CFLAGS)

# Rule to generate object files in the files.o directory
$(OBJ_DIR_O)/%.o: draw/%.c
	# Create directories if they do not exist
	mkdir -p $(OBJ_DIR_O) $(OBJ_DIR_EXE)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Rule to clean up object files and the executable
clean:
	rm -f $(OBJ) $(EXEC)
	rm -f $(LIBRARY)  # Remove shared library as well
	rmdir $(OBJ_DIR_O) $(OBJ_DIR_EXE) $(LIB_DIR) 2>/dev/null || true

# Indicate that clean and run are not files
.PHONY: all clean run
