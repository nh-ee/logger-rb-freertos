
#ifndef UTILS_STM32L4XX_H
#define UTILS_STM32L4XX_H

#include <stdint.h>

/**
 * @brief  Busy-wait delay using inline NOPs (32-bit only).
 * @note   Suitable for CPU load testing, RTOS timing, etc.
 * @param  ms: delay time in milliseconds (approximate)
 */
void cpu_burn_ms( uint32_t ms );

#endif /* UTILS_STM32L4XX_H */
