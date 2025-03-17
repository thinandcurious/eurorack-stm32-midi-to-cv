#pragma once

#include "main.h"
#include "Transport.h"

typedef struct {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} usb_midi_packet_t;

class USBTransport : public Transport {
public:
    void push(usb_midi_packet_t value);
};