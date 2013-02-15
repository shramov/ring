

#CFLAGS=-O3 -g -Wall 
CFLAGS=-O0 -g -Wall 

all: ringtest mttest

ringtest:  test.o ring.o 
	gcc -g test.o ring.o -o ringtest


ring.o:	ring.c ring.h


mttest:	 ring.o mttest.o
	gcc -g mttest.o ring.o -lpthread -o mttest


clean:
	rm -f test.o ring.o

