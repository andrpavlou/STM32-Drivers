#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H


#include <string.h>
#include <stdbool.h>
#include <stdatomic.h> 
#include <stdint.h>

#define RX_BUFFER_SIZE 128U /* Power of 2 */

typedef struct RingBuffer {
    uint8_t data[RX_BUFFER_SIZE];
    _Atomic uint8_t head;
    _Atomic uint8_t tail;
} RingBuffer;



void ring_buffer_init(RingBuffer *ring);
bool ring_buffer_push(RingBuffer *ring, uint8_t byte);
bool ring_buffer_pop(RingBuffer *ring, uint8_t *byte);


#endif