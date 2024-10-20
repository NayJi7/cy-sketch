all:
	gcc -o Louaye/main.exe Louaye/main.c -Isrc/include -Lsrc/lib -lSDL2main -lSDL2
	./Louaye/main.exe