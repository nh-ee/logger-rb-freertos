
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utils_stm32l4xx.h"

#include "task_h.h"

static void task_h_entry( void * pv_params ) {
    /* Unused parameters. */
    ( void ) pv_params;

    uint32_t u32_cnt = 0;
    for( ; ; ) {
        /* Example Task Code */
    	printf( "High Priority Task! Run: %ld\r\n", u32_cnt++ );
    	cpu_burn_ms( 10 );
        vTaskDelay( 250U ); /* delay 2500 ticks */
    }
}


bool task_h_create( void ) {
	bool xret = true;

	BaseType_t xtask_ret = pdPASS;
	xtask_ret = xTaskCreate( task_h_entry,						// Task entry function
				             "TaskH",							// Task tag
				             configMINIMAL_STACK_SIZE,			// Stack depth
				             NULL,								// Params to entry function
				             30U,								// Priority
				             NULL								// Task handle
				           );

	if ( pdPASS != xtask_ret ) {
		xret = false;
	}

	return (xret);
}

