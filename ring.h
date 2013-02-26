#include <stdint.h>
#include <stddef.h>
#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    // mah: see [1],[4]: shouldnt head+tail be volatile?
    /* volatile */ size_t size;
    /* volatile */ size_t head;
    size_t tail;
} ring_header_t;

typedef int32_t ring_size_t; // Negative numbers are needed for skips

typedef struct
{
    ring_header_t * header;
    char * buf;
} ringbuffer_t;


int ring_init(ringbuffer_t *ring, size_t size, void * memory);
int ring_write(ringbuffer_t *ring, void * data, size_t size);
void * ring_next(ringbuffer_t *ring);
ring_size_t ring_next_size(ringbuffer_t *ring);
struct iovec ring_next_iovec(ringbuffer_t *ring);
void ring_shift(ringbuffer_t *ring);
size_t ring_available(const ringbuffer_t *ring);

void ring_dump(ringbuffer_t *ring, const char *name);

#ifdef __cplusplus
}; //extern "C"
#endif
