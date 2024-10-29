#FULL CHATGPT

# Name of the final executable
EXEC = files.exe/main.exe

# Directory for object files
OBJ_DIR = files.o

# List of source files
SRC = draw/main.c draw/form.c

# List of object files (replace .c with .o and add the directory path)
OBJ = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.c=.o)))

# Compilation command
CC = gcc

# Compilation options
CFLAGS = -Wall -Wextra -lSDL2 -lSDL2_gfx -lm

# Default rule to generate the executable
all: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

# Rule to generate object files in the files.o directory
$(OBJ_DIR)/%.o: draw/%.c
#   Create the directory if it does not exist
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Rule to clean up object files and the executable
clean:
	rm -f $(OBJ) $(EXEC)
#   Remove the directory if it is empty
	rmdir $(OBJ_DIR) 2>/dev/null || true  

# Indicate that the clean and run rules are not files
.PHONY: all clean run
