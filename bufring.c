#include "bufring.h"

#include <errno.h>
#include <string.h>

// what is the use case for this being public ?
int bring_write_begin(bringbuffer_t *ring, void ** data, size_t size, int flags)
{
	const size_t sz = size + sizeof(bring_frame_t);
	if (!ring->write) {
		int r = ring_write_begin(ring->ring, &ring->write, sz);
		if (r) return r;
		ring->write_size = sz;
		ring->write_off = 0;
	}
	bring_frame_t * frame = ring->write + ring->write_off;
	if (ring->write_size < ring->write_off + sizeof(bring_frame_t) + size) {
		// Reallocate
		const void * old = ring->write;
		int r = ring_write_begin(ring->ring, &ring->write, ring->write_off + sz);
		if (r) return r;
		if (old != ring->write)
			memmove(ring->write, old, ring->write_off);
		ring->write_size = ring->write_off + sz;
	}
	frame->size = size;
	frame->flags = flags;
	*data = frame + 1;
	return 0;
}

// what is the use case for this being public ?
int bring_write_end(bringbuffer_t *ring, void * data, size_t size)
{
	if (!ring->write)
		return EINVAL;
	bring_frame_t * frame = ring->write;
	frame->size = size;
	ring->write_off = ring->write_off + sizeof(*frame) + frame->size;
	return 0;
}

// add a frame
int bring_writev(bringbuffer_t *ring, ringvec_t *rv)
{
	return bring_write(ring, rv->rv_base, rv->rv_len, rv->rv_flags);
}

int bring_write(bringbuffer_t *ring, const void * data, size_t size, int flags)
{
    void * ptr;
    int r = bring_write_begin(ring, &ptr, size, flags);
    if (r) return r;
    memmove(ptr, data, size);
    return bring_write_end(ring, ptr, size);
}

// finish and send off a multipart message, consisting of zero or more frames.
int bring_write_flush(bringbuffer_t *ring)
{
	int r = ring_write_end(ring->ring, ring->write, ring->write_off);
	ring->write = 0;
	return r;
}

int bring_write_abort(bringbuffer_t *ring)
{
	ring->write = 0;
	return 0;
}

// read a frame without consuming.
int bring_readv(bringbuffer_t *ring, ringvec_t *rv)
{
	return bring_read(ring, &rv->rv_base, &rv->rv_len, &rv->rv_flags);
}

int bring_read(bringbuffer_t *ring, const void ** data, size_t * size, int * flags)
{
	if (!ring->read) {
		int r = ring_read(ring->ring, &ring->read, &ring->read_size);
		if (r) return r;
		ring->read_off = 0;
	}
	if (ring->read_off == ring->read_size)
		return EAGAIN; //XXX?
	const bring_frame_t * frame = ring->read + ring->read_off;
	*data = frame + 1;
	*size = frame->size;
	*flags = frame->flags;
	return 0;
}

// consume a frame.
int bring_shift(bringbuffer_t *ring)
{
	if (!ring->read || ring->read_off == ring->read_size) return EINVAL;
	const bring_frame_t * frame = ring->read + ring->read_off;
	ring->read_off += sizeof(*frame) + frame->size;
	return 0;
}

// consume a multi-frame message, regardless how many frames were consumed.
int bring_read_flush(bringbuffer_t *ring)
{
	if (!ring->read) return EINVAL;
	ring->read = 0;
	return ring_shift(ring->ring);
}

int bring_read_abort(bringbuffer_t *ring)
{
	if (!ring->read) return EINVAL;
	ring->read = 0;
	return 0;
}
