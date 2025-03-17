#include "main.h"
#include "drivers/gpio.h"
#include "stm32l1xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_FS;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/

void NMI_Handler(void) {
    while (1) {
    }
}

void HardFault_Handler(void) {
    while (1) {
    }
}

void MemManage_Handler(void) {
    while (1) {
    }
}

// This function handles Pre-fetch fault, memory access fault.
void BusFault_Handler(void) {
    while (1) {
    }
}

// This function handles Undefined instruction or illegal state.
void UsageFault_Handler(void) {
    while (1) {
    }
}

// This function handles System service call via SWI instruction.
void SVC_Handler(void) {
}

// This function handles Debug monitor.
void DebugMon_Handler(void) {
}

// This function handles Pendable request for system service.
void PendSV_Handler(void) {
}

// This function handles System tick timer.
void SysTick_Handler(void) {
    HAL_IncTick();
}

/******************************************************************************/
/* STM32L1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l1xx.s).                    */
/******************************************************************************/

void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(LEARN_Pin);
}

void USB_LP_IRQHandler(void) {
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
}

void TIM3_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim3);
}

void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart1);
}

void EXTI15_10_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(SWITCH_Pin);
}
