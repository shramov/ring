#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "ring.h"
#include "bufring.h"


void test_bufring(void)
{
	bringbuffer_t ring1 = { .write = 0, .read = 0 };
	bringbuffer_t ring2 = { .write = 0, .read = 0 };
	bringbuffer_t *ro = &ring1, *rw = &ring2;

	ring_init(&ro->ring, 1024, 0);
	ring_init(&rw->ring, 0, ro->ring.header);

	bring_write(rw, "part0", 5, 1);
	bring_write(rw, "part1", 5, 2);

	//ring_dump(&ro->ring, "ro"); ring_dump(&rw->ring, "rw");

	bring_write(rw, "part2", 5, 3);
	bring_write_flush(rw);

	//ring_dump(&ro->ring, "ro"); ring_dump(&rw->ring, "rw");

	const void * ptr;
	size_t size;
	int flags;
	bring_read(ro, &ptr, &size, &flags);
	printf("Data: %zd %.*s\n", size, (int) size, (const char *) ptr);
	bring_shift(ro);
	bring_read(ro, &ptr, &size, &flags);
	printf("Data: %zd %.*s\n", size, (int) size, (const char *) ptr);
	bring_shift(ro);
	bring_read(ro, &ptr, &size, &flags);
	printf("Data: %zd %.*s\n", size, (int) size, (const char *) ptr);
	bring_shift(ro);

	//ring_dump(ro, "ro"); ring_dump(rw, "rw");

	bring_shift_flush(ro);
}

int main(void)
{
	test_bufring();

	ringbuffer_t ring1, ring2;
	ringbuffer_t *ro = &ring1, *rw = &ring2;
	ring_init(ro, 1024, 0);
	ring_init(rw, 0, ro->header);

	ring_dump(ro, "ro"); ring_dump(rw, "rw");

	ring_write(rw, "test", 4);
	ring_dump(ro, "ro"); ring_dump(rw, "rw");

	ring_write(rw, "test", 0);
	ring_dump(ro, "ro"); ring_dump(rw, "rw");

	ring_shift(ro);
	ring_dump(ro, "ro"); ring_dump(rw, "rw");

	ring_shift(ro);
	ring_dump(ro, "ro"); ring_dump(rw, "rw");

	exit(0);
}


#if 0
Ring ro is empty
Ring rw is empty

Data in ro: 4 test
Data in rw: 4 test

Data in ro: 4 test
Data in rw: 4 test

Data in ro: 0 
Data in rw: 0 

Ring ro is empty
Ring rw is empty

#endif
