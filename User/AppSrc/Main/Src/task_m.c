
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utils_stm32l4xx.h"
#include "task_m.h"

static void task_m_entry( void *pv_params ) {
    /* Unused parameters. */
    ( void ) pv_params;

    uint32_t u32_cnt = 0;
    for( ; ; ) {
        /* Example Task Code */
    	printf( "Medium Priority Task! Run: %ld\r\n", u32_cnt++ );
    	cpu_burn_ms( 25 );
        vTaskDelay( 500U ); /* delay 500 ticks */
    }
}

bool task_m_create( void ) {
	bool xret = true;

	BaseType_t xtask_ret = pdPASS;
	xtask_ret = xTaskCreate( task_m_entry,						// Task entry function
				             "TaskM",							// Task tag
				             configMINIMAL_STACK_SIZE,			// Stack depth
				             NULL,								// Params to entry function
				             20U,								// Priority
				             NULL								// Task handle
				           );

	if ( pdPASS != xtask_ret ) {
		xret = false;
	}

	return (xret);
}
