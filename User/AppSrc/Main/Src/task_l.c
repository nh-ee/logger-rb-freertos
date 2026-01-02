
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utils_stm32l4xx.h"
#include "task_l.h"

static void task_l_entry( void * pv_params ) {
    /* Unused parameters. */
    ( void ) pv_params;

    uint32_t u32_cnt = 0;
    for( ; ; ) {
        /* Example Task Code */
    	printf( "Low Priority task! Run: %ld\r\n", u32_cnt++ );
    	cpu_burn_ms( 50 );
        vTaskDelay( 1000U ); /* delay 100 ticks */
    }
}

bool task_l_create( void ) {
	bool xret = true;

	BaseType_t xtask_ret = pdPASS;
	xtask_ret = xTaskCreate( task_l_entry,						// Task entry function
				             "TaskL",							// Task tag
				             configMINIMAL_STACK_SIZE,			// Stack depth
				             NULL,								// Params to entry function
				             10U,								// Priority
				             NULL								// Task handle
				           );

	if ( pdPASS != xtask_ret ) {
		xret = false;
	}

	return (xret);
}
