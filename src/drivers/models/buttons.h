#pragma once

#include "stm32l1xx_hal.h"

typedef void (*button_click_cb_t)();

class Button {
public:
    Button(button_click_cb_t on_click);

    void update();
    void set_physical_state(bool state);
    bool is_pressed();
protected:
    enum class State {
        RELEASED,
        PRESSED
    };

    State physical_state = State::RELEASED;
    State current_state = State::RELEASED;
private:
    button_click_cb_t on_click;

    uint32_t pressed_time = 0;
    uint32_t released_time = 0;
};