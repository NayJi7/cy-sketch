# Name of the final executable
ifeq ($(OS),Windows_NT)
    EXEC = SDL/files.exe/main.exe
    PYTHON = python
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    ECHO = echo -n
else
    EXEC = SDL/files.exe/main
    PYTHON = python3
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    ECHO = echo -n
endif

# Directory for object files and executable
OBJ_DIR_O = SDL/files.o
OBJ_DIR_EXE = SDL/files.exe

# List of source files
SRC = .to_run.c SDL/src/form.c SDL/src/cursorEvents.c SDL/src/formEvents.c SDL/src/colors.c SDL/src/animations.c SDL/src/game.c

# List of object files (replace .c with .o and add the directory path)
OBJ = $(addprefix $(OBJ_DIR_O)/, $(notdir $(SRC:.c=.o)))

# Compilation command
CC = gcc

# Compilation options
CFLAGS = -Wall -Wextra -g -DDEBUG=$(DEBUG)

# Linker flags (SDL2 and SDL2_gfx libraries included)
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm
else
    LDFLAGS = -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm
endif

# Files for logs
SDL_ERROR_LOG = SDL/.error.log
COMPILATOR_ERROR_LOG = COMPILATOR/.error.log

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
.DEFAULT_GOAL := compile_run

# Compile and run in sequence
compile_run: compile run

# Build the executable
all: clean_log $(EXEC)
	$(LOG) ""
	$(LOG) "-#green === Build Completed Successfully ==="
	$(LOG) ""

# Compilation target for the interpreter
compile: clean_log
	$(LOG) ""
	$(LOG) "=== Running Interpreter ==="
	$(LOG) "- Processing .to_compile.dpp..."
	$(SILENT)$(PYTHON) interpreter.py .to_compile.dpp $(INTERPRETER_FLAGS) -n $(NAME) 2>> $(COMPILATOR_ERROR_LOG)

# Debug target (automatically sets DEBUG=1)
debug: export DEBUG=1
debug: compile_run

# Clean error log
clean_log:
	$(LOG) ""
	$(LOG) "=== Initializing Build Environment ==="
	$(LOG) "- Cleaning error log..."
	$(SILENT)$(ECHO) > $(SDL_ERROR_LOG)
	$(SILENT)$(ECHO) > $(COMPILATOR_ERROR_LOG)

# Create necessary directories
create_dirs:
	$(SILENT)$(MKDIR) $(OBJ_DIR_O) $(OBJ_DIR_EXE)

# Link object files to create the executable
$(EXEC): create_dirs $(OBJ)
	$(LOG) ""
	$(LOG) "=== Linking Phase ==="
	$(LOG) "- Linking object files into executable..."
	$(SILENT)$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS) 2>> $(SDL_ERROR_LOG)

# Rule to generate object files from source files in src directory
$(OBJ_DIR_O)/%.o: SDL/src/%.c
	$(LOG) ""
	$(LOG) "=== Compilation Phase ==="
	$(LOG) "- Compiling $<..."
	$(SILENT)$(CC) $(CFLAGS) -c $< -o $@ 2>> $(SDL_ERROR_LOG)

# Rule to generate object files from root directory source files
$(OBJ_DIR_O)/%.o: %.c
	$(LOG) ""
	$(LOG) "=== Compilation Phase ==="
	$(LOG) "- Compiling $<..."
	$(SILENT)$(CC) $(CFLAGS) -c $< -o $@ 2>> $(SDL_ERROR_LOG)

# Run the program and overwrite the run log
run: clean clean_log all
	$(LOG) "-#blue === Launching Application ==="
	$(LOG) "-#blue - Starting the program..."
ifeq ($(OS),Windows_NT)
	$(SILENT)./$(EXEC) || true
else
	$(SILENT)./$(EXEC) || true
endif

# Rule to clean up object files, the executable, and the logs
clean:
	$(LOG) ""
	$(LOG) "=== Cleanup Phase ==="
	$(LOG) "- Removing build artifacts..."
	$(SILENT)$(RM) $(OBJ) $(EXEC) $(SDL_ERROR_LOG) $(COMPILATOR_ERROR_LOG)
	$(SILENT)$(RMDIR) $(OBJ_DIR_O) $(OBJ_DIR_EXE) 2>/dev/null || true  

# Indicate that clean, run, and debug are not files
.PHONY: all clean run clean_log debug compile compile_run create_dirs
