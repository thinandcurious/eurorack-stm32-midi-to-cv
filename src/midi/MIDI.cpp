#include "midi/MIDI.h"

#include <algorithm>

#include "drivers/dac.h"
#include "drivers/gpio.h"
#include "drivers/timer.h"

bool MIDI::on_drum_on(uint8_t note) {
    switch (note) {
        case 36:
            HAL_GPIO_WritePin(GATE1_GPIO_Port, GATE1_Pin, GPIO_PIN_SET);
            break;
        case 37:
            HAL_GPIO_WritePin(GATE2_GPIO_Port, GATE2_Pin, GPIO_PIN_SET);
            break;
        case 38:
            HAL_GPIO_WritePin(GATE3_GPIO_Port, GATE3_Pin, GPIO_PIN_SET);
            break;
        case 39:
            HAL_GPIO_WritePin(GATE4_GPIO_Port, GATE4_Pin, GPIO_PIN_SET);
            break;
        default:
            return false;
    }
    return true;
}

bool MIDI::on_drum_off(uint8_t note) {
    switch (note) {
        case 36:
            HAL_GPIO_WritePin(GATE1_GPIO_Port, GATE1_Pin, GPIO_PIN_RESET);
            break;
        case 37:
            HAL_GPIO_WritePin(GATE2_GPIO_Port, GATE2_Pin, GPIO_PIN_RESET);
            break;
        case 38:
            HAL_GPIO_WritePin(GATE3_GPIO_Port, GATE3_Pin, GPIO_PIN_RESET);
            break;
        case 39:
            HAL_GPIO_WritePin(GATE4_GPIO_Port, GATE4_Pin, GPIO_PIN_RESET);
            break;
        default:
            return false;
    }
    return true;
}

void MIDI::on_start() {
    HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_SET);
    // Start Timer
    __HAL_TIM_CLEAR_IT(TIMER_START, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(TIMER_START);
}

void MIDI::on_clock() {
    if (clock_count == 0) {
        HAL_GPIO_WritePin(CLOCK_GPIO_Port, CLOCK_Pin, GPIO_PIN_SET);
    } else if (clock_count == 12) {
        HAL_GPIO_WritePin(CLOCK_GPIO_Port, CLOCK_Pin, GPIO_PIN_RESET);
    }
    clock_count++;
    clock_count %= 24;
}

void MIDI::on_note_on(uint8_t midi_note, uint8_t velocity) {
    if (midi_note < 24 || midi_note > 96) {
        return;
    }

    float note_voltage = ((float) midi_note - 24.0f) / 12.0f;  // [0,6]
    float velocity_voltage = ((float) velocity / 127.0f) * 5.0f;

    if (polyphony == MONO) {
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) (note_voltage * (4095.0f / DAC_CHANNEL1_MAX_VOLTAGE)));
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) (velocity_voltage * (4095.0f / DAC_CHANNEL2_MAX_VOLTAGE)));
        HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
        HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

        HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_SET);
        // Trigger Timer
        __HAL_TIM_CLEAR_IT(TIMER_TRIGGER, TIM_IT_UPDATE);
        HAL_TIM_Base_Start_IT(TIMER_TRIGGER);

        HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
    } else if (polyphony == POLY) {
        if (polyphony_dac_next == 1) {
            polyphony_dac1_note = midi_note;
            polyphony_dac_next = 2;
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) (note_voltage * (4095.0f / DAC_CHANNEL1_MAX_VOLTAGE)));
            HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
            HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
        } else if (polyphony_dac_next == 2) {
            polyphony_dac2_note = midi_note;
            polyphony_dac_next = 1;
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) (note_voltage * (4095.0f / DAC_CHANNEL2_MAX_VOLTAGE)));
            HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
            HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_SET);
        }
    }

    notes.push_front({.midi_note = midi_note,
                      .voltage = note_voltage,
                      .velocity = velocity_voltage});
}

void MIDI::on_note_off(uint8_t midi_note) {
    notes.remove_if([midi_note](const Note& note) {
        return note.midi_note == midi_note;
    });

    if (polyphony == MONO) {
        if (notes.empty()) {
            HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_RESET);
        } else {
            Note note = notes.front();
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) (note.voltage * (4095.0f / DAC_CHANNEL1_MAX_VOLTAGE)));
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) (note.velocity * (4095.0f / DAC_CHANNEL2_MAX_VOLTAGE)));
            HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
            HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
        }
    } else if (polyphony == POLY) {
        if (polyphony_dac1_note == midi_note) {
            if (notes.empty()) {
                HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_RESET);
                polyphony_dac1_note = 0;
            } else {
                auto item = std::find_if(notes.begin(), notes.end(), [this](const Note& note) {
                    return note.midi_note != polyphony_dac2_note;
                });
                if (item != notes.end()) {
                    Note note = *item;
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) (note.voltage * (4095.0f / DAC_CHANNEL1_MAX_VOLTAGE)));
                    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
                    polyphony_dac1_note = note.midi_note;
                } else {
                    HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_RESET);
                    polyphony_dac1_note = 0;
                }
                polyphony_dac_next = 1;
            }
        } else if (polyphony_dac2_note == midi_note) {
            if (notes.empty()) {
                HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_RESET);
                polyphony_dac2_note = 0;
            } else {
                auto item = std::find_if(notes.begin(), notes.end(), [this](const Note& note) {
                    return note.midi_note != polyphony_dac1_note;
                });
                if (item != notes.end()) {
                    Note note = *item;
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) (note.voltage * (4095.0f / DAC_CHANNEL2_MAX_VOLTAGE)));
                    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
                    polyphony_dac2_note = note.midi_note;
                } else {
                    HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_RESET);
                    polyphony_dac2_note = 0;
                }
                polyphony_dac_next = 2;
            }
        }
    }
}

void MIDI::handle_event(midi_event_t event) {
    if (event.type == Start) {
        on_start();
        return;
    } else if (event.type == Clock) {
        on_clock();
        return;
    }

    if (event.channel == CHANNEL_10) {  // Drum Channel
        switch (event.type) {
            case NoteOn: {
                if (event.data2 == 0) {
                    if (on_drum_off(event.data1)) {
                        return;
                    }
                } else {
                    if (on_drum_on(event.data1)) {
                        return;
                    }
                }
                break;
            }
            case NoteOff: {
                if (on_drum_off(event.data1)) {
                    return;
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    if (is_learning) {
        is_learning = false;
        active_channel = event.channel;
        HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_RESET);
    }

    if (!(active_channel == CHANNEL_ALL || event.channel == active_channel)) {
        return;
    }

    switch (event.type) {
        case NoteOn: {
            uint8_t note = event.data1;
            uint8_t velocity = event.data2;

            if (velocity == 0) {
                on_note_off(note);
            } else {
                on_note_on(note, velocity);
            }
            break;
        }
        case NoteOff: {
            uint8_t note = event.data1;
            on_note_off(note);
            break;
        }
        default:
            break;
    }
}

void MIDI::task() {
    if (main_transport->available()) {
        midi_event_t event = main_transport->read();
        handle_event(event);
    }
    if (secondary_transport->available()) {
        midi_event_t event = secondary_transport->read();
        handle_event(event);
    }
    if (is_learning) {
        if ((HAL_GetTick() - learning_led_tick) > 100) {
            learning_led_tick += 100;
            HAL_GPIO_TogglePin(START_GPIO_Port, START_Pin);
        }
    }
}

void MIDI::start_learn() {
    if (is_learning == true) {
        is_learning = false;
        HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_RESET);
        return;
    }
    is_learning = true;
    notes.clear();
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) 0);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) 0);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLOCK_GPIO_Port, CLOCK_Pin, GPIO_PIN_RESET);

    learning_led_tick = HAL_GetTick();
    HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_SET);
}

void MIDI::set_polyphony(polyphony_t mode) {
    if (polyphony == mode) {
        return;
    }
    polyphony = mode;

    if (polyphony == POLY) {
        if (HAL_TIM_Base_GetState(TIMER_TRIGGER) == HAL_TIM_STATE_BUSY) {
            HAL_TIM_Base_Stop_IT(TIMER_TRIGGER);
        }
        if (notes.size() == 1) {
            Note note = notes.front();
            HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_RESET);
            polyphony_dac1_note = note.midi_note;
            polyphony_dac_next = 2;
        } else if (notes.size() >= 2) {
            auto it = notes.begin();
            Note note = *it;
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) (note.voltage * (4095.0f / DAC_CHANNEL1_MAX_VOLTAGE)));
            HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
            HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
            polyphony_dac1_note = note.midi_note;
            it++;
            note = *it;
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) (note.voltage * (4095.0f / DAC_CHANNEL2_MAX_VOLTAGE)));
            HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
            HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_SET);
            polyphony_dac2_note = note.midi_note;
            polyphony_dac_next = 2;
        }
    } else if (polyphony == MONO) {
        HAL_GPIO_WritePin(TRIGGER_GPIO_Port, TRIGGER_Pin, GPIO_PIN_RESET);
        if (!notes.empty()) {
            Note note = notes.front();
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t) (note.velocity * (4095.0f / DAC_CHANNEL2_MAX_VOLTAGE)));
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) (note.voltage * (4095.0f / DAC_CHANNEL1_MAX_VOLTAGE)));
            HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
            HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
            HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
        }
    }
}