#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "ring.h"
#include "bufring.h"


void test_bufring(void)
{
        int i;
	bringbuffer_t ring1 = { .write = 0, .read = 0 };
	bringbuffer_t ring2 = { .write = 0, .read = 0 };
	bringbuffer_t *ro = &ring1, *rw = &ring2;

	ringvec_t tx[3] = {
	    { .rv_base = "part0", .rv_len = 5, .rv_flags = 1},
	    { .rv_base = "part1", .rv_len = 5, .rv_flags = 2},
	    { .rv_base = "part2", .rv_len = 5, .rv_flags = 3},
	};

	ring_init(&ro->ring, 1024, 0);
	ring_init(&rw->ring, 0, ro->ring.header);

	for (i = 0; i < 3; i++) {
	    bring_write(rw, &tx[i]);
	}
	bring_write_flush(rw);

	int n;
	ringvec_t rx[10] = { };

 	for (n = 0; bring_read(ro, &rx[n]) == 0; n++, bring_shift(ro));

	for (i = 0; i < n; i++) {
	    ringvec_t *v = &rx[i];
	    printf("Data[%d]: %zd '%.*s' %d\n", i, v->rv_len, (int) v->rv_len,
		   (const char *) v->rv_base, v->rv_flags);
	}
	bring_shift_flush(ro);
}

int main(void)
{
	test_bufring();
#if 0
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
#endif
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
