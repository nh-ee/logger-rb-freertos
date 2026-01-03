
#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdint.h>
#include <stdbool.h>

void log_init( void );

void app_log_process( void );

int app_log( const char *fmt, ... );

#endif /* APP_LOG_H */
