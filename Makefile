# Name of the final executable
EXEC = files.exe/main.exe

# Directory for object files and executable
OBJ_DIR_O = files.o
OBJ_DIR_EXE = files.exe

# List of source files
SRC = draw/main.c draw/form.c draw/cursorEvents.c draw/formEvents.c draw/colors.c

# List of object files (replace .c with .o and add the directory path)
OBJ = $(addprefix $(OBJ_DIR_O)/, $(notdir $(SRC:.c=.o)))

# Compilation command
CC = gcc

# Compilation options
CFLAGS = -Wall -Wextra 

# Linker flags (SDL2 and SDL2_gfx libraries included)
LDFLAGS = -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm

# Files for logs
ERROR_LOG = error.log

# Default rule to generate the executable
all: clean_log $(EXEC)

# Clean error log
clean_log:
	@echo -n > $(ERROR_LOG)

# Link object files to create the executable
$(EXEC): $(OBJ)
	mkdir -p $(OBJ_DIR_EXE)
	@$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS) 2>> $(ERROR_LOG)

# Rule to generate object files in the files.o directory
$(OBJ_DIR_O)/%.o: draw/%.c
	mkdir -p $(OBJ_DIR_O) $(OBJ_DIR_EXE)
	@$(CC) $(CFLAGS) -c $< -o $@ 2>> $(ERROR_LOG)

# Run the program and overwrite the run log
run: clean clean_log all
	@./$(EXEC) 

# Rule to clean up object files, the executable, and the logs
clean:
	rm -f $(OBJ) $(EXEC) $(ERROR_LOG)
	rmdir $(OBJ_DIR_O) $(OBJ_DIR_EXE) 2>/dev/null || true  

# Indicate that clean and run are not files
.PHONY: all clean run clean_log
