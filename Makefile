CPPFLAGS := $(shell python-config --cflags) -fPIC
LDFLAGS := -L.
PYTHON := python

#CFLAGS=-O3 -g -Wall 
CFLAGS=-O0 -g -Wall

all: ringtest mttest python

python: libring.so pyring.so

ringtest:  test.o ring.o bufring.o
	gcc -g $^ -o ringtest

libring.so: ring.o bufring.o
	$(CC) -shared -o $@ $^ $(LDFLAGS)

ring.so: pyring.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ -lboost_python -lring

pyring.so: pyring.pyx libring.pxd
	LDFLAGS=$(LDFLAGS) $(PYTHON) setup.py build_ext --inplace

pyring.o: ring.h

ring.o:	ring.c ring.h
lqueue: lqueue.h
lqueue: CXXFLAGS += -lpthread -std=c++11

markerqueue: markerqueue.h
markerqueue: CXXFLAGS += -lpthread -std=c++11


mttest:	 ring.o mttest.o
	gcc -g mttest.o ring.o -lpthread -o mttest


clean:
	rm -f test.o ring.o

