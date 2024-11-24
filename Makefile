# Name of the final executable
EXEC = files.exe/main.exe

# Directory for object files and executable
OBJ_DIR_O = files.o
OBJ_DIR_EXE = files.exe

# List of source files
SRC = draw/main.c draw/form.c

# List of object files (replace .c with .o and add the directory path)
OBJ = $(addprefix $(OBJ_DIR_O)/, $(notdir $(SRC:.c=.o)))

# Compilation command
CC = gcc

# Compilation options (SDL2 and SDL2_gfx libraries included)
CFLAGS = -Wall -Wextra -lSDL2 -lSDL2_gfx -lm

# Default rule to generate the executable
all: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJ)
	mkdir -p $(OBJ_DIR_EXE)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

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
# Remove dirs if empty
	rmdir $(OBJ_DIR_O) $(OBJ_DIR_EXE) 2>/dev/null || true  

# Indicate that clean and run are not files
.PHONY: all clean run
