
#include <stdio.h>
#include <stdarg.h>

#include "stm32l4xx.h"

#include "utils_stm32l4xx.h"

/**
 * @brief  Busy-wait delay using inline NOPs (32-bit only).
 * @note   Suitable for CPU load testing, RTOS timing, etc.
 * @param  ms: delay time in milliseconds (approximate)
 */
void cpu_burn_ms( uint32_t ms ) {
    if ( 0UL == ms ) {
    	return;
    }

    // Approximate cycles to burn for 1 ms, keeping in 32-bit range.
    // e.g., SystemCoreClock = 80 MHz → 80,000 cycles per ms.
    // We use 32-bit math, so it’s fine up to ~50 seconds at 80 MHz.

    uint32_t cycles_per_ms = SystemCoreClock / 1000U;

    // Estimated cycles per loop iteration (~5 cycles per iteration)
    // Using double to avoid float calculation
    const uint32_t cycles_per_loop = 5U;
    uint32_t iterations = (cycles_per_ms / cycles_per_loop) * ms;

    if ( 0U == iterations ) {
        return;
    }

    // Inline assembly loop — runs exactly 'iterations' times
    __asm__ volatile (
        "1: \n\t"
        "   nop \n\t"
        "   subs %[cnt], %[cnt], #1 \n\t"
        "   bne 1b \n\t"
        : [cnt] "+r" (iterations)
        :
        : "cc"
    );
}

