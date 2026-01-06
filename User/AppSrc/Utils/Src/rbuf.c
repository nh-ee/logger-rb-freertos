#include <stdint.h>
#include <stdbool.h>

#include "rbuf.h"

void ring_buffer_init( RingBuffer_t *px_rb, uint8_t *pu8_dbuf, uint32_t u32_size ) {
    px_rb->buffer = pu8_dbuf;
    px_rb->size = u32_size;
    px_rb->wr_idx = 0;
    px_rb->rd_idx = 0;
    px_rb->write_failures = 0;
}

bool ring_buffer_is_empty( const RingBuffer_t *px_rb ) {
    return px_rb->wr_idx == px_rb->rd_idx;
}

bool ring_buffer_is_full( const RingBuffer_t *px_rb ) {
    return ( (px_rb->wr_idx + 1) % px_rb->size ) == px_rb->rd_idx;
}

uint32_t ring_buffer_available_read( const RingBuffer_t *px_rb ) {
    return (px_rb->wr_idx + px_rb->size - px_rb->rd_idx) % px_rb->size;
}

uint32_t ring_buffer_available_write( const RingBuffer_t *px_rb ) {
    return (px_rb->size - 1) - ring_buffer_available_read( px_rb );
}

bool ring_buffer_write( RingBuffer_t *px_rb, uint8_t u8_data ) {
    if ( ring_buffer_is_full( px_rb ) ) {
        px_rb->write_failures++;
        return ( false );
    }

    px_rb->buffer[px_rb->wr_idx] = u8_data;
    px_rb->wr_idx = (px_rb->wr_idx + 1) % px_rb->size;

    return ( true );
}

bool ring_buffer_write_multiple( RingBuffer_t *px_rb,
                                 const uint8_t *pu8_dbuf, uint32_t u32_len ) {
	if ( ring_buffer_available_write( px_rb ) < u32_len ) {
		px_rb->write_failures++;
		return ( false );
	}

	for ( uint32_t idx = 0; idx < u32_len; idx++ ) {
	    px_rb->buffer[px_rb->wr_idx] = pu8_dbuf[idx];
	    px_rb->wr_idx = (px_rb->wr_idx + 1) % px_rb->size;
	}

	return ( true );
}

bool ring_buffer_read( RingBuffer_t *px_rb, uint8_t *pu8_data ) {
    if ( ring_buffer_is_empty( px_rb ) ) {
        return ( false );
    }

    *pu8_data = px_rb->buffer[px_rb->rd_idx];
    px_rb->rd_idx = (px_rb->rd_idx + 1) % px_rb->size;

    return ( true );
}
