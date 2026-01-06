
#include <stdio.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "app_log.h"
#include "rbuf.h"


static pfn_time_ms_t pfn_log_time_ms = NULL;

static pfn_log_flush_t pfn_log_flush = NULL;

static bool sx_log_transport_busy = true;

static uint32_t su32_dma_tx_len = 0;

#define app_log	printf

#define AL_BUF_SIZE							( 1024 )
#define MSG_FMT_BUF_SIZE					( 128 + 12 )

static uint8_t su8_al_dbuf[AL_BUF_SIZE]      = { 0 };
static uint8_t su8_msg_buf[MSG_FMT_BUF_SIZE] = { 0 };
static RingBuffer_t sx_al_rb;
static SemaphoreHandle_t sx_al_mutex = NULL;

static int u32_to_str( uint8_t *pu8_buf, uint32_t u32_val ) {
    int idx = 0;

    if ( 0 == u32_val ) {
		pu8_buf[idx++] = '0';
    }
    else {
    	// Temporary buffer for the digits
        char tmp_buf[10];
        int tmp_idx = 0;

        while ( u32_val > 0 ) {
			tmp_buf[tmp_idx++] = '0' + (u32_val % 10);
			u32_val /= 10;
        }

        while ( tmp_idx > 0 ) {
			pu8_buf[idx++] = tmp_buf[--tmp_idx];
        }
    }

    return ( idx );
}

bool log_init( app_log_if_t *px_log_if ) {
	bool x_ret = true;

	if ( NULL != px_log_if->pfn_time ) {
		pfn_log_time_ms = px_log_if->pfn_time;
	}

	if ( NULL != px_log_if->pfn_flush ) {
		pfn_log_flush = px_log_if->pfn_flush;
	}
	else {
		// Log transport backend can't be NULL
		x_ret = false;
	}

	ring_buffer_init( &sx_al_rb, su8_al_dbuf, AL_BUF_SIZE );

	sx_al_mutex = xSemaphoreCreateMutex();
	if ( NULL == sx_al_mutex ) {
		x_ret = false;
	}

	return ( x_ret );
}


int app_log( const char *fmt, ... ) {

	if ( NULL == sx_al_mutex ) {
		return 0;
	}

	uint32_t msg_len = 0;
	// TODO: Add configurable delay
	if ( xSemaphoreTake( sx_al_mutex, portMAX_DELAY ) ) {
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

		if ( false == ring_buffer_write_multiple( &sx_al_rb, su8_msg_buf, msg_len ) ) {
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

void app_log_transport_done( void ) {
	sx_al_rb.rd_idx = ( sx_al_rb.rd_idx + su32_dma_tx_len ) % sx_al_rb.size;

	sx_log_transport_busy = false;
}
