
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

/**
 * @brief Function pointer type for a function that transfer logged data
 * @param[in] pu8_data pointer to the data buffer
 * @param[in] u32_len length of data to be transferred
 * @return true on success, false otherwise
 */
typedef bool (*pfn_log_flush_t)( const uint8_t *pu8_data, uint32_t u32_len );

/**
 * @brief Interface structure for the logging module. Application should be
 * responsible to provide implementation details as per requirements.
 */
typedef struct app_log_if {
	pfn_time_ms_t   pfn_time;
	pfn_log_flush_t pfn_flush;
} app_log_if_t;

void log_init( app_log_if_t *px_log_if );

void app_log_process( void );

void app_log_transport_done( void );

int app_log( const char *fmt, ... );

#endif /* APP_LOG_H */
