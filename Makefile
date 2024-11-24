# Nom de l'exécutable final
EXEC = files.exe/main.exe

# Répertoires
OBJ_DIR_O = files.o
OBJ_DIR_EXE = files.exe
LIB_DIR = files.lib

# Fichiers sources
SRC = draw/main.c draw/form.c
LIBRARY = $(LIB_DIR)/libform.so

# Fichiers objets
OBJ = $(addprefix $(OBJ_DIR_O)/, $(notdir $(SRC:.c=.o)))

# Compiler et options
CC = gcc
CFLAGS = -Wall -Wextra -fPIC -I/usr/include/python3.12  # Inclure Python.h
LDFLAGS = -L$(LIB_DIR) -lform -lSDL2 -lSDL2_gfx -lm -lpython3.12  # Lier Python

# Règle par défaut
all: $(EXEC) $(LIBRARY)

# Compile l'exécutable
$(EXEC): $(OBJ) $(LIBRARY)
	mkdir -p $(OBJ_DIR_EXE)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Crée la bibliothèque partagée
$(LIBRARY): draw/form.c
	mkdir -p $(LIB_DIR)
	$(CC) -shared -fPIC -o $(LIBRARY) draw/form.c -lSDL2 -lSDL2_gfx -lm

# Compile les fichiers objets
$(OBJ_DIR_O)/%.o: draw/%.c
	mkdir -p $(OBJ_DIR_O)
	$(CC) $(CFLAGS) -c $< -o $@

# Lancer le programme
run: $(EXEC)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(EXEC)

# Nettoyage
clean:
	rm -f $(OBJ) $(EXEC) $(LIBRARY)
	rmdir $(OBJ_DIR_O) $(OBJ_DIR_EXE) $(LIB_DIR) 2>/dev/null || true

# Indiquer que ces règles ne sont pas des fichiers
.PHONY: all clean run
