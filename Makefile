CC=gcc

ifeq ($(OS),Windows_NT)
	DELETE=del
else
	DELETE=rm -f
endif


all: buildShell

buildTextEditor:
	$(CC) -o bin/textedit.out src/main.c

clean:
	$(DELETE) *.o *.out