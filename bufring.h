#ifndef __BUFFERED_RING_H__
#define __BUFFERED_RING_H__

#include "ring.h"

typedef struct {
	ringbuffer_t * ring;
	void * write;
	size_t write_size;
	size_t write_off;
	const void * read;
	size_t read_size;
	size_t read_off;
} bringbuffer_t;

typedef struct {
    const void *rv_base;
    size_t rv_len;
    int    rv_flags;
} ringvec_t;

typedef struct {
	int32_t size;
	int32_t flags;
} bring_frame_t;

int bring_write_begin(bringbuffer_t *ring, void ** data, size_t size, int flags);
int bring_write_end(bringbuffer_t *ring, void * data, size_t size);
int bring_write(bringbuffer_t *ring, const void * data, size_t size, int flags);
int bring_writev(bringbuffer_t *ring, ringvec_t *rv);
int bring_write_flush(bringbuffer_t *ring);
int bring_write_abort(bringbuffer_t *ring);

int bring_read(bringbuffer_t *ring, const void **data, size_t *size, int *flags);
int bring_readv(bringbuffer_t *ring, ringvec_t *rv);
int bring_shift(bringbuffer_t *ring);
int bring_read_flush(bringbuffer_t *ring);
int bring_read_abort(bringbuffer_t *ring);

#endif//__BUFFERED_RING_H__
