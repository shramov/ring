# vim: sts=4 sw=4 et

from libc.errno cimport EAGAIN
from libc.string cimport memcpy
from cpython.buffer cimport PyBuffer_FillInfo
from cpython.bytes cimport PyBytes_AsString, PyBytes_Size, PyBytes_FromStringAndSize

from libring cimport *

cdef class mview:
    cdef void *base
    cdef int size

    def __cinit__(self, long base, size):
        self.base = <void *>base
        self.size = size

    def __getbuffer__(self, Py_buffer *view, int flags):
        r = PyBuffer_FillInfo(view, self, self.base, self.size, 0, flags)
        view.obj = self

cdef class Ring:
    cdef ringbuffer_t _ring

    def __cinit__(self, int size):
        if ring_init(&self._ring, size, NULL):
            raise RuntimeError("Failed to initialize ringbuffer")

    def __dealloc__(self):
        ring_free(&self._ring)

    def write(self, s):
        cdef void * ptr
        cdef size_t size = PyBytes_Size(s)
        cdef int r = ring_write_begin(&self._ring, &ptr, size)
        if r:
            if r != EAGAIN:
                raise RuntimeError("Ring write failed")
            return False
        memcpy(ptr, PyBytes_AsString(s), size)
        ring_write_end(&self._ring, ptr, size)
        return True

    def read(self):
        cdef const void * ptr
        cdef size_t size
        cdef int r = ring_read(&self._ring, &ptr, &size)
        if r:
            if r != EAGAIN:
                raise RuntimeError("Ring read failed")
            return None
        return memoryview(mview(<long>ptr, size))

    def shift(self):
        ring_shift(&self._ring)

    def __iter__(self):
        return RingIter(self)

cdef class RingIter:
    cdef ringiter_t _iter

    def __cinit__(self, ring):
        if ring_iter_init(&(<Ring>(ring))._ring, &self._iter):
            raise RuntimeError("Failed to initialize ring iter")

    def read(self):
        cdef const void * ptr
        cdef size_t size
        cdef int r = ring_iter_read(&self._iter, &ptr, &size)
        if r:
            if r != EAGAIN:
                raise RuntimeError("Ring read failed")
            return None
        return memoryview(mview(<long>ptr, size))

    def shift(self):
        ring_iter_shift(&self._iter)

    def __iter__(self):
        return self

    def __next__(self):
        r = self.read()
        if r is None:
            raise StopIteration("Ring is empty")
        s = memoryview(r.tobytes())
        self.shift()
        return s

    def next(self): return self.__next__()
