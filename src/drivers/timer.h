#ifndef __DRIVERS_TIMER_H__
#define __DRIVERS_TIMER_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERS_TIMER_H__