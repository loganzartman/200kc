CC = gcc -std=c99 -Ofast

ifeq ($(OS), Windows_NT)
	LIBS = -lmingw32 -lSDL2main -lSDL2
	LIBDIR = $(shell pwd)/SDL2/i686-w64-mingw32/lib
	INCDIR = $(shell pwd)/SDL2/include
else
	LIBS = -lSDL2main -lSDL2 -lm
	LIBDIR = /usr/lib
	INCDIR = /usr/include/SDL2
endif

SOURCES = game.c graphics.c Timer.c particles.c util.c

all:
	$(CC) $(SOURCES) -o bin/game -I"$(INCDIR)" -L"$(LIBDIR)" $(LIBS)

clean:
	rm -rf bin
	mkdir bin
	cp "$(LIBDIR)/../bin/SDL2.dll" "bin/SDL2.dll"
