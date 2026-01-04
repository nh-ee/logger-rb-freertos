
#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Function pointer type for a function that returns the system time in milliseconds
 * @param None
 * @return system time in milliseconds as uint32_t
 */
typedef uint32_t (*pfn_time_ms_t) ( void );

void log_init( pfn_time_ms_t pfn );

void app_log_process( void );

int app_log( const char *fmt, ... );

#endif /* APP_LOG_H */
