
#include <stdio.h>
#include <stdarg.h>

#include "stm32l4xx.h"

#include "app_log.h"
#include "rbuf.h"


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
