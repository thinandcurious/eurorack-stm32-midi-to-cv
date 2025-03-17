#include "buttons.h"

#define DEBOUNCE_MS 100

Button::Button(button_click_cb_t on_click) : on_click(on_click) {}

void Button::update() {
    uint32_t now = HAL_GetTick();

    // PRESSED ACTION
    if( physical_state == State::PRESSED && current_state == State::RELEASED && (now - released_time) > DEBOUNCE_MS ){
        current_state = State::PRESSED;
        pressed_time = now;
    }

    // RELEASED ACTION
    if( physical_state == State::RELEASED && current_state == State::PRESSED && (now - pressed_time) > DEBOUNCE_MS ){
        current_state = State::RELEASED;
        released_time = now;
        on_click();
    }
}

void Button::set_physical_state(bool state) {
    physical_state = state ? State::PRESSED : State::RELEASED;
}

bool Button::is_pressed() {
    return current_state == State::PRESSED;
}