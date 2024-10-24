all:
	gcc -o draw/main.exe draw/main.c -Isrc/include -Lsrc/lib -lSDL2main -lSDL2 -lSDL2_gfx
	./exe/main.exe