#include "main.h"

#include "Deque.h"
#include "drivers/dac.h"
#include "drivers/gpio.h"
#include "drivers/timer.h"
#include "drivers/uart.h"
#include "drivers/usb/usb_device.h"
#include "drivers/usb/class/midi/usbd_midi.h"
#include "drivers/models/buttons.h"
#include "drivers/models/switches.h"
#include "midi/MIDI.h"
#include "midi/transport/SerialTransport.h"
#include "midi/transport/USBTransport.h"

void SystemClock_Config(void);

SerialTransport serial_transport = SerialTransport(&huart1);
USBTransport usb_transport = USBTransport();

MIDI midi = MIDI(&usb_transport, &serial_transport);

static void on_learn_button_click();
Button learn_button = Button(on_learn_button_click);

static void on_polyphony_switch_change(bool state);
Switch polyphony_switch = Switch(on_polyphony_switch_change);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_DAC_Init();
    // MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_USB_DEVICE_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 0);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

    if( HAL_GPIO_ReadPin(SWITCH_GPIO_Port, SWITCH_Pin) == GPIO_PIN_SET ){
        polyphony_switch.set_physical_state(true);
    }else{
        polyphony_switch.set_physical_state(false);
    }
    
    serial_transport.begin();

    // for( int i = 0; i < 3; i++ ) {
    //     HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_SET);
    //     HAL_Delay(250);
    //     HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_RESET);
    //     HAL_Delay(250);
    // }
    
    while (1) {
        learn_button.update();
        polyphony_switch.update();
        midi.task();
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length) {
    while( usb_rx_buffer_length && *usb_rx_buffer != 0x00 ){
        uint8_t byte0 = *usb_rx_buffer++;
        uint8_t byte1 = *usb_rx_buffer++;
        uint8_t byte2 = *usb_rx_buffer++;
        uint8_t byte3 = *usb_rx_buffer++;

        usb_midi_packet_t usb_midi_packet = {
            .byte0 = byte0,
            .byte1 = byte1,
            .byte2 = byte2,
            .byte3 = byte3,
        };

        usb_transport.push(usb_midi_packet);

        usb_rx_buffer_length -= 4;
    }
}

static void on_learn_button_click() {
    midi.start_learn();
}

static void on_polyphony_switch_change(bool state) {
    if( state == true ){
        midi.set_polyphony(MONO);
    }else if( state == false ){
        midi.set_polyphony(POLY);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if( huart == &huart1 ){
        serial_transport.irq();
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if( GPIO_Pin == LEARN_Pin ){
        if( HAL_GPIO_ReadPin(LEARN_GPIO_Port, LEARN_Pin) == GPIO_PIN_RESET ){
            learn_button.set_physical_state(true);
        }else{
            learn_button.set_physical_state(false);
        }
    }
    else if( GPIO_Pin == SWITCH_Pin ){
        if( HAL_GPIO_ReadPin(SWITCH_GPIO_Port, SWITCH_Pin) == GPIO_PIN_SET ){
            polyphony_switch.set_physical_state(true);
        }else{
            polyphony_switch.set_physical_state(false);
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if( htim == TIMER_TRIGGER ){
        HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_RESET);
        HAL_TIM_Base_Stop_IT(TIMER_TRIGGER);
    }
    else if( htim == TIMER_START ){
        HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_RESET);
        HAL_TIM_Base_Stop_IT(TIMER_START);
    }
}

void Error_Handler(void) {
    __disable_irq();
    HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CLOCK_GPIO_Port, CLOCK_Pin, GPIO_PIN_SET);
    while (1) {
        
    }
}
