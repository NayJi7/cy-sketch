all:
	gcc -o draw/main.exe draw/main.c -lSDL2 -lSDL2_gfx
	./draw/main.exe