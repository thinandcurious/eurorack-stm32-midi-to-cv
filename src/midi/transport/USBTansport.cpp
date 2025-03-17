#include "USBTransport.h"

void USBTransport::push(usb_midi_packet_t usb_midi_packet) {
    uint8_t code_index_number = usb_midi_packet.byte0 & 0x0F;

    midi_event_t midi_event = {
        .type = get_event_type(usb_midi_packet.byte1),
        .channel = CHANNEL_ALL,
        .data1 = 0,
        .data2 = 0,
    };

    switch(code_index_number) {
        // 1 bytes messages
        case 0x5: // Single-byte System Common Message or SysEx ends with following single byte
        case 0xF: // Single Byte
            break;
        
        // 3 bytes messages
        case 0x8: // Note Off
        case 0x9: // Note On
        case 0xA: // Aftertouch Poly
        case 0xB: // Control Change
        case 0xE: // Pitch Bend
            midi_event.data1 = usb_midi_packet.byte2;
            midi_event.data2 = usb_midi_packet.byte3;
            break;

        // 2 bytes messages
        case 0xC: // Program Change
        case 0xD: // Channel Pressure
            midi_event.data1 = usb_midi_packet.byte2;
            break;        
    }

    switch( midi_event.type ){
        case NoteOff: case NoteOn: case AfterTouchPoly: case ControlChange: case ProgramChange: case AfterTouchChannel: case PitchBend: {
            midi_event.channel = get_event_channel(usb_midi_packet.byte1);
            break;
        }
        default: {
            break;
        }
    }

    midi_events.push(midi_event);
}