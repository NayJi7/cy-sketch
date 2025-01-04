# Name of the final executable
EXEC = files.exe/main.exe

# Directory for object files and executable
OBJ_DIR_O = files.o
OBJ_DIR_EXE = files.exe

# List of source files
SRC = draw/main.c draw/form.c draw/cursorEvents.c draw/formEvents.c draw/colors.c draw/animations.c

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

# Debug mode control (use make DEBUG=1 for debug mode)
DEBUG ?= 0
ifeq ($(DEBUG),1)
  SILENT =
  LOG = @echo
else
  SILENT = @
  LOG = @true
endif

# Default rule to generate the executable
all: clean_log $(EXEC)
	$(LOG) "\n=== Build Completed Successfully ===\n"

# Debug target (automatically sets DEBUG=1)
debug: export DEBUG=1
debug: all

# Clean error log
clean_log:
	$(LOG) "\n=== Initializing Build Environment ==="
	$(LOG) "→ Cleaning error log..."
	$(SILENT)echo -n > $(ERROR_LOG)

# Link object files to create the executable
$(EXEC): $(OBJ)
	$(LOG) "\n=== Linking Phase ==="
	$(LOG) "→ Linking object files into executable..."
	$(SILENT)mkdir -p $(OBJ_DIR_EXE)
	$(SILENT)$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS) 2>> $(ERROR_LOG)

# Rule to generate object files in the files.o directory
$(OBJ_DIR_O)/%.o: draw/%.c
	$(LOG) "\n=== Compilation Phase ==="
	$(LOG) "→ Compiling $<..."
	$(SILENT)mkdir -p $(OBJ_DIR_O) $(OBJ_DIR_EXE)
	$(SILENT)$(CC) $(CFLAGS) -c $< -o $@ 2>> $(ERROR_LOG)

# Run the program and overwrite the run log
run: clean clean_log all
	$(LOG) "\n=== Launching Application ==="
	$(LOG) "→ Starting the program..."
	$(SILENT)./$(EXEC) || true

# Rule to clean up object files, the executable, and the logs
clean:
	$(LOG) "\n=== Cleanup Phase ==="
	$(LOG) "→ Removing build artifacts..."
	$(SILENT)rm -f $(OBJ) $(EXEC) $(ERROR_LOG)
	$(SILENT)rm -rf $(OBJ_DIR_O) $(OBJ_DIR_EXE) 2>/dev/null || true  

# Indicate that clean, run, and debug are not files
.PHONY: all clean run clean_log debug
