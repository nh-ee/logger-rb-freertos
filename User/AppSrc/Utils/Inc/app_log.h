
#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Function pointer type for a function that returns the system time in milliseconds
 * @param None
 * @return system time in milliseconds (uint32_t type)
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
	pfn_time_ms_t   pfn_time;		///< Pointer to function get system time
	pfn_log_flush_t pfn_flush;		///< Pointer to function that sends log stream to physical medium 
} app_log_if_t;

/**
 * @brief API initiates logging module
 * @param[in] px_log_if Pointer to logging module interface
 * @return true on success, false otherwise
 */
bool log_init( app_log_if_t *px_log_if );

/**
 * @brief Enqueues a formatted log message
 * @param[in] fmt  Format string (printf-style)
 * @param[in] ...  Additional arguments corresponding to the format string
 * @return total length of message written
 * 
 * @note This function will replace standard printf function if enabled with
 * configuration switch. So, user can use printf for customized log feature
 */
int app_log( const char *fmt, ... );

/**
 * @brief API triggers log flush to the configured physical medium
 * @param None
 * @return None
 * 
 * @note This API must be called periodically to dequeue log buffer through
 * configured physical medium
 */
void app_log_process( void );

/**
 * @brief API resets ongoing transfer flag for next one and increment ring buffer
 * read index
 * @param None
 * @return None
 * 
 * @note This API must be called upon completion of data transfer on physical medium
 */
void app_log_transport_done( void );

#endif /* APP_LOG_H */
