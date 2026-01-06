
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Ring buffer data structure definition
 */
typedef struct RingBuffer {
    uint8_t  *buffer;           //< User-provided storage
    uint32_t size;            	//< Total buffer size
    uint32_t wr_idx;         	//< Write index
    uint32_t rd_idx;         	//< Read index
    uint32_t write_failures; 	//< Count of failed writes (buffer full)
} RingBuffer_t ;

/**
 * @brief Initialize a ring buffer
 * @param[in] px_rb Pointer to ring buffer data structure
 * @param[in] pu8_dbuf Pointer to data buffer
 * @param[in] u32_size Total length of data buffer
 * @return None
 */
void ring_buffer_init( RingBuffer_t *px_rb, uint8_t *pu8_dbuf, uint32_t u32_size );

/**
 * @brief API checks if ring buffer is empty
 * @param[in] px_rb Pointer to ring buffer data structure
 * @return true on empty, false otherwise
 */
bool ring_buffer_is_empty( const RingBuffer_t *px_rb );

/**
 * @brief API checks if ring buffer is full
 * @param[in] px_rb Pointer to ring buffer data structure
 * @return true on full, false otherwise
 */
bool ring_buffer_is_full( const RingBuffer_t *px_rb );

/**
 * @brief API checks if data is available to read from ring buffer
 * @param[in] px_rb Pointer to ring buffer data structure
 * @return amount of available bytes can be read (uint32_t type)
 */
uint32_t ring_buffer_available_read( const RingBuffer_t *px_rb );

/**
 * @brief API checks if data can be written into ring buffer
 * @param[in] px_rb Pointer to ring buffer data structure
 * @return amount of available bytes can be written (uint32_t type)
 */
uint32_t ring_buffer_available_write( const RingBuffer_t *px_rb );

/**
 * @brief API writes single byte into ring buffer
 * @param[in] px_rb Pointer to ring buffer data structure
 * @param[in] u8_data data to write
 * @return true on successful write, false otherwise
 */
bool ring_buffer_write( RingBuffer_t *px_rb, uint8_t u8_data );

/**
 * @brief API writes multiple bytes into ring buffer
 * @param[in] px_rb Pointer to ring buffer data structure
 * @param[in] pu8_dbuf Pointer to data buffer to be written
 * @param[in] u32_len length of data to be written
 * @return true on successful write, false otherwise
 */
bool ring_buffer_write_multiple( RingBuffer_t *px_rb,
                                 const uint8_t *pu8_dbuf, uint32_t u32_len );

/**
 * @brief API reads single byte from ring buffer
 * @param[in] px_rb Pointer to ring buffer data structure
 * @param[in] pu8_data pointer to data storing varible (uint8_t type)
 * @return true on successful read, false otherwise
 */
bool ring_buffer_read( RingBuffer_t *px_rb, uint8_t *pu8_data );

#endif /* RING_BUFFER_H */
