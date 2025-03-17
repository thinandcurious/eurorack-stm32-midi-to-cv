#ifndef __DRIVERS_DAC_H__
#define __DRIVERS_DAC_H__

#include "main.h"

#define DAC_CHANNEL1_MAX_VOLTAGE 6.626f
#define DAC_CHANNEL2_MAX_VOLTAGE 6.627f

#ifdef __cplusplus
extern "C" {
#endif

extern DAC_HandleTypeDef hdac;

void MX_DAC_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERS_DAC_H__