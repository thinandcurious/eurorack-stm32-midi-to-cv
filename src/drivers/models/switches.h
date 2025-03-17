#pragma once

#include "stm32l1xx_hal.h"

typedef void (*switch_change_cb_t)(bool);

class Switch {
public:
    Switch(switch_change_cb_t on_change);

    void update();
    void set_physical_state(bool state);
    bool get_state();
protected:
    bool physical_state = true;
    bool current_state = true;
private:
    switch_change_cb_t on_change;
    uint32_t changed_time = 0;
};