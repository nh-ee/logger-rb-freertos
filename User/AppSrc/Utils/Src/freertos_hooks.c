
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"


/*********************************************************************
*
*       vApplicationMallocFailedHook
*
*  Function description
*    Called if a call to pvPortMalloc() fails because there
*    is insufficient free memory available in the FreeRTOS heap.
*    pvPortMalloc() is called internally by FreeRTOS API functions
*    that create tasks, queues, software timers, and semaphores.
*    The size of the FreeRTOS heap is set by the configTOTAL_HEAP_SIZE
*    configuration constant in FreeRTOSConfig.h
*
*/
void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void) {
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

/*********************************************************************
*
*       vApplicationStackOverflowHook
*
*  Function description
*    Run time stack overflow checking is performed if
*    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.
*    This hook function is called if a stack overflow is detected.
*/
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName) {
  ( void ) pcTaskName;
  ( void ) xTask;
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

/*********************************************************************
*
*       vApplicationIdleHook
*
*  Function description
*    This function is called on each cycle of the idle task.
*    In this case it does nothing useful, other than report
*    the amount of FreeRTOS heap that remains unallocated.
*
*/
void vApplicationIdleHook(void) {
#if configSUPPORT_DYNAMIC_ALLOCATION == 1
  volatile size_t xFreeHeapSpace;

  xFreeHeapSpace = xPortGetFreeHeapSize();
  if( xFreeHeapSpace > 100 ) {
    //
    // By now, the kernel has allocated everything it is going to, so
    // if there is a lot of heap remaining unallocated then
    // the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
    // reduced accordingly.
  }
#endif
}

/*********************************************************************
*
*       vMainConfigureTimerForRunTimeStats
*
*  Function description
*    The Blinky build configuration does not include run time
*    stats gathering, however, the Full and Blinky build configurations
*    share a FreeRTOSConfig.h file.
*    Therefore, dummy run time stats functions need to be defined
*    to keep the linker happy.
*
*/
void vMainConfigureTimerForRunTimeStats(void);
void vMainConfigureTimerForRunTimeStats(void) {
}

/*********************************************************************
*
*       ulMainGetRunTimeCounterValue
*
*/
unsigned long ulMainGetRunTimeCounterValue(void);
unsigned long ulMainGetRunTimeCounterValue(void) {
  return 0UL;
}

/*********************************************************************
*
*       vApplicationTickHook
*
*  Function description
*    A tick hook is used by the "Full" build configuration.
*    The Full and blinky build configurations share a FreeRTOSConfig.h
*    header file, so this simple build configuration also has to define
*    a tick hook - even though it does not actually use it for anything.
*
*/
void vApplicationTickHook(void);
void vApplicationTickHook(void) {
}

