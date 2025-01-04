# Name of the final executable
EXEC = SDL/files.exe/main.exe

# Directory for object files and executable
OBJ_DIR_O = SDL/files.o
OBJ_DIR_EXE = SDL/files.exe

# List of source files
SRC = .to_run.c SDL/src/form.c SDL/src/cursorEvents.c SDL/src/formEvents.c SDL/src/colors.c SDL/src/animations.c

# List of object files (replace .c with .o and add the directory path)
OBJ = $(addprefix $(OBJ_DIR_O)/, $(notdir $(SRC:.c=.o)))

# Compilation command
CC = gcc

# Compilation options
CFLAGS = -Wall -Wextra -g -DDEBUG=$(DEBUG)

# Linker flags (SDL2 and SDL2_gfx libraries included)
LDFLAGS = -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm

# Files for logs
SDL_ERROR_LOG = SDL/.error.log
COMPILATOR_ERROR_LOG = compilator/.error.log

# Debug mode control (use make DEBUG=1 for debug mode)
DEBUG ?= 0
ifeq ($(DEBUG),1)
  SILENT =
  LOG = @echo
  INTERPRETER_FLAGS = -d
else
  SILENT = @
  LOG = @true
  INTERPRETER_FLAGS =
endif

# Name for the program (defaults to Draw++)
NAME ?= "Draw ++"

# Default target: compile and run
.DEFAULT_GOAL := compil_run

# Compile and run in sequence
compil_run: compil run

# Build the executable
all: clean_log $(EXEC)
	$(LOG) "\n=== Build Completed Successfully ===\n"

# Compilation target for the interpreter
compil: clean_log
	$(LOG) "\n=== Running Interpreter ==="
	$(LOG) "→ Processing .to_compil.dpp..."
	$(SILENT)python3 interpreter.py .to_compil.dpp $(INTERPRETER_FLAGS) -n $(NAME) 2>> $(COMPILATOR_ERROR_LOG)

# Debug target (automatically sets DEBUG=1)
debug: export DEBUG=1
debug: compil_run

# Clean error log
clean_log:
	$(LOG) "\n=== Initializing Build Environment ==="
	$(LOG) "→ Cleaning error log..."
	$(SILENT)echo -n > $(SDL_ERROR_LOG)
	$(SILENT)echo -n > $(COMPILATOR_ERROR_LOG)

# Create necessary directories
create_dirs:
	$(SILENT)mkdir -p $(OBJ_DIR_O) $(OBJ_DIR_EXE)

# Link object files to create the executable
$(EXEC): create_dirs $(OBJ)
	$(LOG) "\n=== Linking Phase ==="
	$(LOG) "→ Linking object files into executable..."
	$(SILENT)$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS) 2>> $(SDL_ERROR_LOG)

# Rule to generate object files from source files in src directory
$(OBJ_DIR_O)/%.o: SDL/src/%.c
	$(LOG) "\n=== Compilation Phase ==="
	$(LOG) "→ Compiling $<..."
	$(SILENT)$(CC) $(CFLAGS) -c $< -o $@ 2>> $(SDL_ERROR_LOG)

# Rule to generate object files from root directory source files
$(OBJ_DIR_O)/%.o: %.c
	$(LOG) "\n=== Compilation Phase ==="
	$(LOG) "→ Compiling $<..."
	$(SILENT)$(CC) $(CFLAGS) -c $< -o $@ 2>> $(SDL_ERROR_LOG)

# Run the program and overwrite the run log
run: clean clean_log all
	$(LOG) "\033[32m\n=== Launching Application ==="
	$(LOG) "→ Starting the program...\033[0m"
	$(SILENT)./$(EXEC) || true

# Rule to clean up object files, the executable, and the logs
clean:
	$(LOG) "\n=== Cleanup Phase ==="
	$(LOG) "→ Removing build artifacts..."
	$(SILENT)rm -f $(OBJ) $(EXEC) $(SDL_ERROR_LOG) $(COMPILATOR_ERROR_LOG)
	$(SILENT)rm -rf $(OBJ_DIR_O) $(OBJ_DIR_EXE) 2>/dev/null || true  

# Indicate that clean, run, and debug are not files
.PHONY: all clean run clean_log debug compil compil_run create_dirs
