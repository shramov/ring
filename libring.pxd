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

cdef extern from "bufring.h":
    ctypedef struct bringbuffer_t:
        ringbuffer_t * ring

    ctypedef struct ringvec_t:
        void  *rv_base
        size_t rv_len
        int    rv_flags

    int bring_write_begin(bringbuffer_t *ring, void ** data, size_t size, int flags)
    int bring_write_end(bringbuffer_t *ring, void * data, size_t size)
    int bring_write(bringbuffer_t *ring, const void * data, size_t size, int flags)
    int bring_writev(bringbuffer_t *ring, ringvec_t *rv)
    int bring_write_flush(bringbuffer_t *ring)
    int bring_write_abort(bringbuffer_t *ring)

    int bring_read(bringbuffer_t *ring, const void **data, size_t *size, int *flags)
    int bring_readv(bringbuffer_t *ring, ringvec_t *rv)
    int bring_shift(bringbuffer_t *ring)
    int bring_read_flush(bringbuffer_t *ring)
    int bring_read_abort(bringbuffer_t *ring)
