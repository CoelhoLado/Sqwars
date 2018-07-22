CFLAGS=-std=c11 -Wall -Wextra -pedantic -g -lSDL2main -lSDL2 -lm
FICHEIROS=sqwars.c Makefile
EXECUTAVEL=sqwars

$(EXECUTAVEL): sqwars.o
	gcc -o $(EXECUTAVEL) sqwars.c -O2 -lSDL2main -lSDL2 -lm `sdl2-config --cflags --libs`
