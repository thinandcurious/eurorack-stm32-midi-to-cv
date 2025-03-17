#include "SerialTransport.h"

void SerialTransport::begin() {
    HAL_UART_Receive_IT(huart, &uart_rx_value, 1);
}

void SerialTransport::irq() {
    uart_rx_buffer[uart_rx_buffer_write_index] = uart_rx_value;
    uart_rx_buffer_write_index = (uart_rx_buffer_write_index + 1) % 64;
    HAL_UART_Receive_IT(huart, &uart_rx_value, 1);
}

bool SerialTransport::available() {
    while( uart_rx_buffer_read_index != uart_rx_buffer_write_index ){
        parse(uart_rx_buffer[uart_rx_buffer_read_index]);
        uart_rx_buffer_read_index = (uart_rx_buffer_read_index + 1) % 64;
    }
    return Transport::available();
}

void SerialTransport::parse(uint8_t byte) {
    // Status byte
    if( byte & 0x80 ){
        pending_event.type = get_event_type(byte);
        pending_event.channel = CHANNEL_ALL;
        pending_event.data1 = 0;
        pending_event.data2 = 0;

        switch( pending_event.type ){
            case Start: case Continue: case Stop: case Clock: case Tick: case ActiveSensing: case SystemReset: case TuneRequest: {
                // 1 bytes messages
                pending_event_remaining_bytes = 0;
                midi_events.push(pending_event);
                break;
            }
            case ProgramChange: case AfterTouchChannel: case TimeCodeQuarterFrame: case SongSelect: {
                // 2 bytes messages
                pending_event_remaining_bytes = 1;
                break;
            }
            case NoteOn: case NoteOff: case ControlChange: case PitchBend: case AfterTouchPoly: case SongPosition: {
                // 3 bytes messages
                pending_event_remaining_bytes = 2;
                break;
            }
            case SystemExclusiveStart: case SystemExclusiveEnd: {
                // The message can be any length >= 3
                pending_event_remaining_bytes = 0;
                break;
            }
            case InvalidType:
            default: {
                // Something is wrong, ignore
                pending_event_remaining_bytes = 0;
                return;
            }
        }

        switch( pending_event.type ){
            case NoteOff: case NoteOn: case AfterTouchPoly: case ControlChange: case ProgramChange: case AfterTouchChannel: case PitchBend: {
                pending_event.channel = get_event_channel(byte);
                break;
            }
            default: {
                break;
            }
        }
    }
    // Data byte
    else{
        if( pending_event_remaining_bytes == 0 ){
            // We received data bytes but didn't expect any, could be an unhandled event type, ignore
            return;
        }

        switch( pending_event.type ){
            case ProgramChange: case AfterTouchChannel: case TimeCodeQuarterFrame: case SongSelect: {
                pending_event.data1 = byte;
                break;
            }
            case NoteOn: case NoteOff: case ControlChange: case PitchBend: case AfterTouchPoly: case SongPosition: {
                if( pending_event_remaining_bytes == 2 ){
                    pending_event.data1 = byte;
                }else{
                    pending_event.data2 = byte;
                }
                break;
            }
            default: {
                // We received data bytes for an event type that doesn't use them, ignore
                pending_event_remaining_bytes = 0;
                return;
            }
        }

        pending_event_remaining_bytes--;

        if( pending_event_remaining_bytes == 0 ){
            midi_events.push(pending_event);
        }
    }
}