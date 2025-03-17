#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32l1xx_hal.h"

#define TIMER_START &htim3
#define TIMER_TRIGGER &htim2

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);
void USBD_MIDI_DataInHandler(uint8_t * usb_rx_buffer, uint8_t usb_rx_buffer_length);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__