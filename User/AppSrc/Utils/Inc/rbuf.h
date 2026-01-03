
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct RingBuffer {
    uint8_t  *buffer;          // User-provided storage
    uint32_t size;            	// Total buffer size
    uint32_t wr_idx;         	// Write index
    uint32_t rd_idx;         	// Read index
    uint32_t write_failures; 	// Count of failed writes (buffer full)
} RingBuffer_t ;

// Initialize ring buffer
static inline void ring_buffer_init( RingBuffer_t *rb,
                            uint8_t *buffer, uint32_t size ) {
    rb->buffer = buffer;
    rb->size = size;
    rb->wr_idx = 0;
    rb->rd_idx = 0;
    rb->write_failures = 0;
}

// Check if buffer is empty
static inline bool ring_buffer_is_empty( const RingBuffer_t *rb ) {
    return rb->wr_idx == rb->rd_idx;
}

// Check if buffer is full (one slot is unused)
static inline bool ring_buffer_is_full( const RingBuffer_t *rb ) {
    return ( (rb->wr_idx + 1) % rb->size ) == rb->rd_idx;
}

// Bytes available to read
static inline uint32_t ring_buffer_available_read( const RingBuffer_t *rb ) {
    return (rb->wr_idx + rb->size - rb->rd_idx) % rb->size;
}

// Bytes available to write
static inline uint32_t ring_buffer_available_write( const RingBuffer_t *rb ) {
    return (rb->size - 1) - ring_buffer_available_read( rb );
}

// Write one byte to buffer
static inline bool ring_buffer_write( RingBuffer_t *rb, uint8_t data ) {
    if ( ring_buffer_is_full( rb ) ) {
        rb->write_failures++;
        return false;
    }

    rb->buffer[rb->wr_idx] = data;
    rb->wr_idx = (rb->wr_idx + 1) % rb->size;
    return true;
}

// Write multiple bytes to buffer
static inline bool ring_buffer_write_multiple( RingBuffer_t *rb, const uint8_t *data, uint32_t w_len ) {
	if ( ring_buffer_available_write( rb ) < w_len ) {
		rb->write_failures++;
		return false;
	}

	for ( uint32_t idx = 0; idx < w_len; idx++ ) {
	    rb->buffer[rb->wr_idx] = data[idx];
	    rb->wr_idx = (rb->wr_idx + 1) % rb->size;
	}

	return true;
}

// Read one byte from buffer
static inline bool ring_buffer_read( RingBuffer_t *rb, uint8_t *data ) {
    if ( ring_buffer_is_empty( rb ) ) {
        return false;
    }

    *data = rb->buffer[rb->rd_idx];
    rb->rd_idx = (rb->rd_idx + 1) % rb->size;
    return true;
}

#endif /* RING_BUFFER_H */
