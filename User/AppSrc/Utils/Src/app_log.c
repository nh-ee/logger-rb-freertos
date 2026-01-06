
#include <stdio.h>
#include <stdarg.h>

#include "stm32l4xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "app_log.h"
#include "rbuf.h"

extern UART_HandleTypeDef huart1;

#define APP_LOG_SCHEME_NONE				0
#define APP_LOG_SCHEME_DBUF				1
#define APP_LOG_SCHEME_RBUF				2
#define APP_LOG_SCHEME_RBUF_IDLE		3

#define APP_LOG_SCHEME					( APP_LOG_SCHEME_RBUF_IDLE )

static bool sx_log_transport_busy = true;

static pfn_time_ms_t pfn_log_time_ms = NULL;

static pfn_log_flush_t pfn_log_flush = NULL;

#if ( APP_LOG_SCHEME == APP_LOG_SCHEME_DBUF )

#define UART_BUFFER_SIZE 32
static uint8_t uart_tx_buffer_1[UART_BUFFER_SIZE] = { 0 };
static uint8_t uart_tx_buffer_2[UART_BUFFER_SIZE] = { 0 };
static uint8_t *current_buffer = uart_tx_buffer_1;
static uint8_t *transmit_buffer = uart_tx_buffer_1;
volatile uint16_t uart_tx_idx = 0;

#elif ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF )

#define LOG_BUF_SIZE 128
static uint8_t log_dbuf[LOG_BUF_SIZE] = { 0 };
static RingBuffer_t log_rb;
static uint32_t su32_dma_tx_len = 0;

#elif ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF_IDLE )

#define app_log	printf

#define AL_BUF_SIZE				( 1024 )
#define MSG_FMT_BUF_SIZE		( 128 + 12 )
static uint8_t su8_al_dbuf[AL_BUF_SIZE] = { 0 };
static uint8_t su8_msg_buf[MSG_FMT_BUF_SIZE] = { 0 };
static RingBuffer_t sx_al_rb;
static SemaphoreHandle_t sx_al_mutex = NULL;
static uint32_t su32_dma_tx_len = 0;

#else

#error "Define logging module buffer scheme"

#endif

int u32_to_str( uint8_t *buf, uint32_t value ) {
    int idx = 0;

    if ( 0 == value ) {
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

    return ( idx );
}

#if ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF )
void log_flush( void ) {
    if ( true == sx_log_transport_busy ) {
        return;
    }

    uint32_t u32_rb_avail = ring_buffer_available_read( &log_rb );
    if ( 0 == u32_rb_avail ) {
        return;
    }

    /* DMA can only send a contiguous block */
    uint32_t rd_idx = log_rb.rd_idx;
    uint32_t wr_idx = log_rb.wr_idx;

    if ( wr_idx > rd_idx ) {
    	su32_dma_tx_len = wr_idx - rd_idx;
    }
    else {
    	su32_dma_tx_len = log_rb.size - rd_idx;
    }

    sx_log_transport_busy = true;

    HAL_UART_Transmit_DMA( &huart1, &log_rb.buffer[rd_idx], su32_dma_tx_len );
}
#endif

#if ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF_IDLE )
int app_log( const char *fmt, ... ) {

	// Get lock
	if ( NULL == sx_al_mutex ) {
		return 0;
	}

	uint32_t msg_len = 0;
	uint32_t ts_len = 0;

	if ( NULL != pfn_log_time_ms ) {
		uint32_t u32_time_ms = pfn_log_time_ms();
		ts_len = u32_to_str( &su8_msg_buf[msg_len], u32_time_ms );
		msg_len += ts_len;
		su8_msg_buf[msg_len++] = ' ';
	}

	va_list args;
	va_start( args, fmt );
	msg_len += vsnprintf( (char *)&su8_msg_buf[msg_len], MSG_FMT_BUF_SIZE - ts_len - 1, fmt, args );
	va_end( args );

	if ( xSemaphoreTake( sx_al_mutex, portMAX_DELAY ) ) {
		if ( !ring_buffer_write_multiple( &sx_al_rb, su8_msg_buf, msg_len ) ) {
			msg_len = 0;
		}
		xSemaphoreGive( sx_al_mutex );
	}

    return ( msg_len );
}

void app_log_process( void ) {
	if ( true == sx_log_transport_busy ) {
		return;
	}

	uint32_t u32_rb_avail = ring_buffer_available_read( &sx_al_rb );
	if ( 0 == u32_rb_avail ) {
		return;
	}

	/* DMA can only send a contiguous block */
	uint32_t rd_idx = sx_al_rb.rd_idx;
	uint32_t wr_idx = sx_al_rb.wr_idx;

	if ( wr_idx > rd_idx ) {
		su32_dma_tx_len = wr_idx - rd_idx;
	}
	else {
		su32_dma_tx_len = sx_al_rb.size - rd_idx;
	}

	sx_log_transport_busy = true;

	//HAL_UART_Transmit_DMA( &huart1, &sx_al_rb.buffer[rd_idx], su32_dma_tx_len );
	if ( NULL != pfn_log_flush ) {
		pfn_log_flush( &sx_al_rb.buffer[rd_idx], su32_dma_tx_len );
	}
}
#else
void app_log_process( void ) {

}
#endif

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_putchar( int ch ) {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the LPUART1 and Loop until the end of transmission */
    //HAL_UART_Transmit( &huart1, (uint8_t *)&ch, 1, 0xFFFF );

#if ( APP_LOG_SCHEME == APP_LOG_SCHEME_DBUF )
	// If the current buffer is full, swap the buffers and start DMA transfer
	if ( uart_tx_idx >= UART_BUFFER_SIZE || ch == '\n' ) {
		// Swap buffers if DMA isn't already in progress
		if ( !sx_log_transport_busy ) {
			// Start DMA transfer of the current buffer
			sx_log_transport_busy = true;

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
#elif ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF )
	ring_buffer_write( &log_rb, ch );
	if ( ch == '\n' || ring_buffer_available_read( &log_rb ) > 10  ) {
		log_flush();
	}
#else

#endif

    return ( ch );
}

void log_init(app_log_if_t *px_log_if ) {
	if ( NULL != px_log_if->pfn_time ) {
		pfn_log_time_ms = px_log_if->pfn_time;
	}

	if ( NULL != px_log_if->pfn_flush ) {
		pfn_log_flush = px_log_if->pfn_flush;
	}

#if ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF )
	ring_buffer_init( &log_rb, log_dbuf, LOG_BUF_SIZE );
#elif ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF_IDLE )
	ring_buffer_init( &sx_al_rb, su8_al_dbuf, AL_BUF_SIZE );
	sx_al_mutex = xSemaphoreCreateMutex();
#else
#endif
}

void app_log_transport_done( void ) {
#if ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF )
	/* Advance rd_idx */
	log_rb.rd_idx = ( log_rb.rd_idx + su32_dma_tx_len ) % log_rb.size;
#elif ( APP_LOG_SCHEME == APP_LOG_SCHEME_RBUF_IDLE )
	sx_al_rb.rd_idx = ( sx_al_rb.rd_idx + su32_dma_tx_len ) % sx_al_rb.size;
#else
#endif

	sx_log_transport_busy = false;
}
