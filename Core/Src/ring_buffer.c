#include "ring_buffer.h"

void ring_buffer_init(RingBuffer *ring)
{
    ring->head = 0;
    ring->tail = 0;
}


bool ring_buffer_push(RingBuffer *ring, uint8_t byte) 
{
    uint8_t next_tail = ((ring->tail + 1) & (RX_BUFFER_SIZE - 1));
    if (next_tail == ring->head) {
        return false;
    }

    ring->data[ring->tail] = byte;
    ring->tail = next_tail;

    return true;
}

bool ring_buffer_pop(RingBuffer *ring, uint8_t *byte) 
{
    if (ring->head == ring->tail) {
        return false;
    }

    *byte = ring->data[ring->head];
    ring->head = ((ring->head + 1) & (RX_BUFFER_SIZE - 1));

    return true;
}