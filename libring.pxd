# vim: sts=4 sw=4 et

from libc.stdint cimport uint64_t

cdef extern from "ring.h":
    ctypedef struct ringbuffer_t:
        void * header
        char * buf

    ctypedef struct ringiter_t:
        const ringbuffer_t * ring 
        uint64_t generation
        size_t * offset

    int ring_init(ringbuffer_t *ring, size_t size, void * memory)
    void ring_free(ringbuffer_t *ring)

    int ring_write_begin(ringbuffer_t *ring, void ** data, size_t size)
    int ring_write_end(ringbuffer_t *ring, void * data, size_t size)
    int ring_write(ringbuffer_t *ring, const void * data, size_t size)

    int ring_read(const ringbuffer_t *ring, const void **data, size_t *size)
    int ring_shift(ringbuffer_t *ring)

    int ring_iter_init(const ringbuffer_t *ring, ringiter_t *iter)
    int ring_iter_shift(ringiter_t *iter)
    int ring_iter_read(const ringiter_t *iter, const void **data, size_t *size)

