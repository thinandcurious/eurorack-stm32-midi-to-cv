#ifndef __DRIVERS_GPIO_H__
#define __DRIVERS_GPIO_H__

#include "main.h"

#define LEARN_Pin GPIO_PIN_2
#define LEARN_GPIO_Port GPIOA
#define LEARN_EXTI_IRQn EXTI2_IRQn
#define GATE4_Pin GPIO_PIN_3
#define GATE4_GPIO_Port GPIOA
#define GATE2_Pin GPIO_PIN_6
#define GATE2_GPIO_Port GPIOA
#define TRIGGER_Pin GPIO_PIN_7
#define TRIGGER_GPIO_Port GPIOA
#define CLOCK_Pin GPIO_PIN_0
#define CLOCK_GPIO_Port GPIOB
#define START_Pin GPIO_PIN_1
#define START_GPIO_Port GPIOB
#define GATE_Pin GPIO_PIN_2
#define GATE_GPIO_Port GPIOB
#define GATE1_Pin GPIO_PIN_10
#define GATE1_GPIO_Port GPIOB
#define GATE3_Pin GPIO_PIN_12
#define GATE3_GPIO_Port GPIOB
#define SWITCH_Pin GPIO_PIN_13
#define SWITCH_GPIO_Port GPIOB
#define SWITCH_EXTI_IRQn EXTI15_10_IRQn
#define USB_SENSE_Pin GPIO_PIN_8
#define USB_SENSE_GPIO_Port GPIOA

#ifdef __cplusplus
extern "C" {
#endif

void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERS_GPIO_H__