
#include <stdio.h>
#include <stdarg.h>

#include "stm32l4xx.h"

#include "app_log.h"

extern UART_HandleTypeDef huart1;

bool app_log_transport( const uint8_t *pu8_data, uint32_t u32_len ) {
	bool x_ret = true;

	HAL_UART_Transmit_DMA( &huart1, pu8_data, u32_len );

	return (x_ret);
}


void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart ) {
	if ( huart == &huart1 ) {
		app_log_transport_done();
	}
}


