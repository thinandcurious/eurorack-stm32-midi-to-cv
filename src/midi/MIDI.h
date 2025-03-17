#ifndef __MIDI_MIDI_H
#define __MIDI_MIDI_H

#include <list>

#include "main.h"
#include "transport/Transport.h"

enum polyphony_t : uint8_t {
    MONO = 0,
    POLY
};

class MIDI {
   public:
    MIDI(Transport* main_transport, Transport* secondary_transport) : main_transport(main_transport), secondary_transport(secondary_transport) {}

    void task();

    void start_learn();
    void set_polyphony(polyphony_t mode);

   private:
    struct Note {
        uint8_t midi_note;
        float voltage;
        float velocity;
    };

    Transport* main_transport;
    Transport* secondary_transport;

    std::list<Note> notes;

    uint8_t clock_count = 0;

    polyphony_t polyphony = MONO;
    uint8_t polyphony_dac_next = 1;
    uint8_t polyphony_dac1_note = 0;
    uint8_t polyphony_dac2_note = 0;
    channel_t active_channel = CHANNEL_ALL;
    bool is_learning = 0;
    uint32_t learning_led_tick = 0;

    void on_start();
    void on_clock();
    bool on_drum_on(uint8_t note);
    bool on_drum_off(uint8_t note);
    void on_note_on(uint8_t note, uint8_t velocity);
    void on_note_off(uint8_t note);

    void handle_event(midi_event_t event);
};

#endif