

CFLAGS=-O3 -g -Wall 


ringtest:  test.o ring.o 
	gcc -g test.o ring.o -o ringtest


ring.o:	ring.c ring.h



clean:
	rm -f test.o ring.o

