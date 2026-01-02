#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 128   				// Define the buffer size (can be adjusted)

typedef struct {
    uint8_t buffer[BUFFER_SIZE];   		// Buffer to hold data
    volatile uint32_t head;        		// Write pointer
    volatile uint32_t tail;        		// Read pointer
    volatile uint32_t write_failures;  	// Count of write failures due to buffer being full
} RingBuffer;

// Initialize the ring buffer
void ring_buffer_init(RingBuffer* rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->write_failures = 0;  // Initialize failure count to zero
}

// Check if the ring buffer is empty
bool ring_buffer_is_empty(RingBuffer* rb) {
    return rb->head == rb->tail;
}

// Check if the ring buffer is full
bool ring_buffer_is_full(RingBuffer* rb) {
    return ((rb->head + 1) % BUFFER_SIZE) == rb->tail;
}

// Write data to the ring buffer
bool ring_buffer_write(RingBuffer* rb, uint8_t data) {
    if (ring_buffer_is_full(rb)) {
        rb->write_failures++;  // Increment failure counter when write fails
        return false; // Buffer is full, cannot write
    }
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % BUFFER_SIZE;
    return true;
}

// Read data from the ring buffer
bool ring_buffer_read(RingBuffer* rb, uint8_t* data) {
    if (ring_buffer_is_empty(rb)) {
        return false; // Buffer is empty, cannot read
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;
    return true;
}
