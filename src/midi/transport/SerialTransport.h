#ifndef __MIDI_SERIAL_TRANSPORT_H__
#define __MIDI_SERIAL_TRANSPORT_H__

#include <deque>
#include "main.h"
#include "Transport.h"

class SerialTransport : public Transport {
public:
    SerialTransport(UART_HandleTypeDef* huart) : huart(huart) {};
    void begin();
    void irq();
    bool available() override;
private:
    UART_HandleTypeDef* huart;
    uint8_t uart_rx_value;
    uint8_t uart_rx_buffer[64];
    uint8_t uart_rx_buffer_write_index = 0;
    uint8_t uart_rx_buffer_read_index = 0;

    midi_event_t pending_event;
    uint32_t pending_event_remaining_bytes = 0;
    void parse(uint8_t byte);
};

#endif // __MIDI_SERIAL_TRANSPORT_H__