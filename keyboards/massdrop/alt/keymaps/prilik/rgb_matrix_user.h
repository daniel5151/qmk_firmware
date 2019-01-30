#pragma once

#include "quantum.h"

typedef enum {
  LED_BUILTIN = 0, // use massdrop builtin modes
  LED_SNAKE,       // play snake using the arrow keys!
} alt_led_mode_t;

bool rgb_matrix_process_record_user(uint16_t keycode, keyrecord_t *record);
void rgb_matrix_set_mode(alt_led_mode_t mode);
