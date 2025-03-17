#include "switches.h"

#define DEBOUNCE_MS 100

Switch::Switch(switch_change_cb_t on_change) : on_change(on_change) {}

void Switch::update() {
    uint32_t now = HAL_GetTick();

    if( physical_state != current_state && (now - changed_time) > DEBOUNCE_MS ){
        current_state = physical_state;
        changed_time = now;
        on_change(current_state);
    }
}

void Switch::set_physical_state(bool state) {
    physical_state = state;
}

bool Switch::get_state() {
    return current_state;
}