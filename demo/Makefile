# demo makefile

all:
	gcc -o ./demo demo.c -I../include/ -L../ -g -lneutron -lgraphics $(shell pkg-config --libs freetype2) -fsanitize=address
