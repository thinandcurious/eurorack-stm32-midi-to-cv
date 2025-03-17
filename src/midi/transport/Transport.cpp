#include "Transport.h"

bool Transport::available() {
    return !midi_events.empty();
}

midi_event_t Transport::read() {
    return midi_events.pop();
}