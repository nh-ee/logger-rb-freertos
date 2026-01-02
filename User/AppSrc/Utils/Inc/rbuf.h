
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct RingBuffer {
    uint8_t  *buffer;               // User-provided storage
    uint32_t  size;                 // Total buffer size
    volatile uint32_t head;         // Write index
    volatile uint32_t tail;         // Read index
    volatile uint32_t write_failures; // Count of failed writes (buffer full)
} RingBuffer_t ;

/* Initialize ring buffer */
static inline void ring_buffer_init( RingBuffer_t *rb,
                            uint8_t *buffer, uint32_t size ) {
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->write_failures = 0;
}

/* Check if buffer is empty */
static inline bool ring_buffer_is_empty( const RingBuffer_t *rb ) {
    return rb->head == rb->tail;
}

/* Check if buffer is full (one slot is unused) */
static inline bool ring_buffer_is_full( const RingBuffer_t *rb ) {
    return ( (rb->head + 1) % rb->size ) == rb->tail;
}

/* Bytes available to read */
static inline uint32_t ring_buffer_available_read( const RingBuffer_t *rb ) {
    return (rb->head + rb->size - rb->tail) % rb->size;
}

/* Bytes available to write */
static inline uint32_t ring_buffer_available_write( const RingBuffer_t *rb ) {
    return (rb->size - 1) - ring_buffer_available_read( rb );
}

/* Write one byte to buffer */
static inline bool ring_buffer_write( RingBuffer_t *rb, uint8_t data ) {
    if ( ring_buffer_is_full( rb ) ) {
        rb->write_failures++;
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    return true;
}

/* Read one byte from buffer */
static inline bool ring_buffer_read( RingBuffer_t *rb, uint8_t *data ) {
    if ( ring_buffer_is_empty( rb ) ) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}

#endif /* RING_BUFFER_H */
