
#include <stdio.h>
#include <stdarg.h>

#include "stm32l4xx.h"

#include "app_log.h"
#include "rbuf.h"

extern UART_HandleTypeDef huart1;
// Get lock

int u32_to_str( char *buf, uint32_t value ) {
    int idx = 0;

    if ( value == 0 ) {
        buf[idx++] = '0';
    }
    else {
    	// Temporary buffer for the digits
        char tmp_buf[10];
        int tmp_idx = 0;

        while ( value > 0 ) {
        	tmp_buf[tmp_idx++] = '0' + (value % 10);
            value /= 10;
        }

        while ( tmp_idx > 0 ) {
            buf[idx++] = tmp_buf[--tmp_idx];
        }
    }

    return idx;
}

#define USE_RBUF		0

volatile uint8_t tx_in_progress = 1;

#if USE_RBUF
#define UART_BUFFER_SIZE 32
uint8_t uart_tx_buffer_1[UART_BUFFER_SIZE];
uint8_t uart_tx_buffer_2[UART_BUFFER_SIZE];

uint8_t *current_buffer = uart_tx_buffer_1;
uint8_t *transmit_buffer = uart_tx_buffer_1;
volatile uint16_t uart_tx_idx = 0;
#else
#define LOG_BUF_SIZE 128
static uint8_t log_dbuf[LOG_BUF_SIZE] = { 0 };
static RingBuffer_t log_rb;
static uint32_t dma_tx_length = 0;

void log_flush(void) {
    if ( tx_in_progress ) {
        return;
    }

    uint32_t available = ring_buffer_available_read( &log_rb );
    if ( available == 0 ) {
        return;
    }

    /* DMA can only send a contiguous block */
    uint32_t tail = log_rb.tail;
    uint32_t head = log_rb.head;

    if ( head > tail ) {
        dma_tx_length = head - tail;
    }
    else {
        dma_tx_length = log_rb.size - tail;
    }

    tx_in_progress = 1;

    HAL_UART_Transmit_DMA( &huart1, &log_rb.buffer[tail], dma_tx_length );
}

#endif

void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart ) {
	if ( huart != &huart1 ) {
		return;
	}

#if (USE_RBUF == 0)
	/* Advance tail */
	log_rb.tail = ( log_rb.tail + dma_tx_length ) % log_rb.size;
#endif

	tx_in_progress = 0;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_putchar(int ch) {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the LPUART1 and Loop until the end of transmission */
    //HAL_UART_Transmit( &huart1, (uint8_t *)&ch, 1, 0xFFFF );

#if USE_RBUF
	// If the current buffer is full, swap the buffers and start DMA transfer
	if ( uart_tx_idx >= UART_BUFFER_SIZE || ch == '\n' ) {
		// Swap buffers if DMA isn't already in progress
		if ( !tx_in_progress ) {
			// Start DMA transfer of the current buffer
			tx_in_progress = 1;

			transmit_buffer = current_buffer;

			HAL_UART_Transmit_DMA( &huart1, transmit_buffer, uart_tx_idx );

			// Reset the buffer index for the next transmission
			uart_tx_idx = 0;

			// Swap the buffers: fill the current buffer with new data
			if ( current_buffer == uart_tx_buffer_1 ) {
				current_buffer = uart_tx_buffer_2;
			}
			else {
				current_buffer = uart_tx_buffer_1;
			}
		}
	}

	// Add the character to the current buffer
	current_buffer[uart_tx_idx++] = (uint8_t) ch;
#else

	ring_buffer_write( &log_rb, ch );

	if ( ch == '\n' || ring_buffer_available_read( &log_rb ) > 10  ) {
		log_flush();
	}

#endif

    return ch;
}

void log_init( void ) {
#if (USE_RBUF == 0)
	ring_buffer_init( &log_rb, log_dbuf, LOG_BUF_SIZE );
#endif
}

int app_log( const char *fmt, ... ) {

	// Get lock

    char log_message[256];
    char timestamp_buf[20];
    va_list args;
    int len = 0;

    // Get the timestamp and convert it to a string
    uint32_t timestamp = HAL_GetTick();
    int ts_len = u32_to_str( timestamp_buf, timestamp );
    timestamp_buf[ts_len++] = ' ';

    // Add timestamp to log message
    for ( int i = 0; i < ts_len; i++ ) {
        log_message[len++] = timestamp_buf[i];
    }

    // Process the format string and append it after the timestamp
    va_start( args, fmt );
    len += vsnprintf( &log_message[len], sizeof(log_message) - len, fmt, args );
    va_end( args );

    return 0;
}

void app_log_process( void ) {

}
