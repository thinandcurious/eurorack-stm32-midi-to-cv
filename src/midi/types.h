#pragma once

#include "main.h"

enum channel_t : uint8_t {
    CHANNEL_1 = 0,
    CHANNEL_2 = 1,
    CHANNEL_3 = 2,
    CHANNEL_4 = 3,
    CHANNEL_5 = 4,
    CHANNEL_6 = 5,
    CHANNEL_7 = 6,
    CHANNEL_8 = 7,
    CHANNEL_9 = 8,
    CHANNEL_10 = 9,
    CHANNEL_11 = 10,
    CHANNEL_12 = 11,
    CHANNEL_13 = 12,
    CHANNEL_14 = 13,
    CHANNEL_15 = 14,
    CHANNEL_16 = 15,
    CHANNEL_ALL = 255
};

enum event_status_t : uint8_t {
    InvalidType           = 0x00,    // For notifying errors
    NoteOff               = 0x80,    // Channel Message - Note Off (1000)
    NoteOn                = 0x90,    // Channel Message - Note On (1001)
    AfterTouchPoly        = 0xA0,    // Channel Message - Polyphonic AfterTouch
    ControlChange         = 0xB0,    // Channel Message - Control Change / Channel Mode
    ProgramChange         = 0xC0,    // Channel Message - Program Change
    AfterTouchChannel     = 0xD0,    // Channel Message - Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    // Channel Message - Pitch Bend
    SystemExclusive       = 0xF0,    // System Exclusive
    SystemExclusiveStart  = SystemExclusive,   // System Exclusive Start
    TimeCodeQuarterFrame  = 0xF1,    // System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    // System Common - Song Position Pointer
    SongSelect            = 0xF3,    // System Common - Song Select
    Undefined_F4          = 0xF4,
    Undefined_F5          = 0xF5,
    TuneRequest           = 0xF6,    // System Common - Tune Request
    SystemExclusiveEnd    = 0xF7,    // System Exclusive End
    Clock                 = 0xF8,    // System Real Time - Timing Clock
    Undefined_F9          = 0xF9,
    Tick                  = Undefined_F9, // System Real Time - Timing Tick (1 tick = 10 milliseconds)
    Start                 = 0xFA,    // System Real Time - Start
    Continue              = 0xFB,    // System Real Time - Continue
    Stop                  = 0xFC,    // System Real Time - Stop
    Undefined_FD          = 0xFD,
    ActiveSensing         = 0xFE,    // System Real Time - Active Sensing
    SystemReset           = 0xFF,    // System Real Time - System Reset
};

typedef struct {
    event_status_t type;
    channel_t channel;
    uint8_t data1;
    uint8_t data2;
} midi_event_t;

inline event_status_t get_event_type(uint8_t status_byte) {
    if( (status_byte  < 0x80) ||
        (status_byte == Undefined_F4) ||
        (status_byte == Undefined_F5) ||
        (status_byte == Undefined_FD) ){
        return InvalidType; // Data bytes and undefined.
    }
    if( status_byte < 0xF0 ){
        // Channel message, remove channel nibble.
        return (event_status_t) (status_byte & 0xF0);
    }
    return (event_status_t) status_byte;
}


inline channel_t get_event_channel(uint8_t status_byte) {
    return (channel_t) (status_byte & 0x0F);
}