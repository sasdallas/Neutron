# demo makefile

CC ?= gcc

all:
	$(CC) -o ./demo demo.c -I../include/ -L../ -g -lneutron $(shell pkg-config --libs freetype2) -fsanitize=address

clean:
	-rm demo
