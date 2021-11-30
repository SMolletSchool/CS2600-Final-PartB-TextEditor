CC=gcc

ifeq ($(OS),Windows_NT)
	DELETE=del
else
	DELETE=rm -f
endif


all: kilo

kilo: src/kilo.c
	$(CC) src/kilo.c -o bin/kilo.out -Wall -Wextra -pedantic -std=c99

clean:
	$(DELETE) bin/kilo.out