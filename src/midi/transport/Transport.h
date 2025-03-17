#ifndef __MIDI_TRANSPORT_H__
#define __MIDI_TRANSPORT_H__

#include "Deque.h"
#include "midi/types.h"

class Transport {
public:
    virtual bool available();
    virtual midi_event_t read();
protected:
    Deque<midi_event_t, 64> midi_events;
};

#endif // __MIDI_TRANSPORT_H__