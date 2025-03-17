#ifndef __DRIVERS_UART_H__
#define __DRIVERS_UART_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERS_UART_H__