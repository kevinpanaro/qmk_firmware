/* Copyright 2021 Kevin Panaro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "print.h"

// Defines names for use in layer keycodes and the keymap
// if you add a layer here, make sure to update the num_of_layers definition
enum layer_names {
    _BASE=0,
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(                     \
        KC_0    ,                         \
        KC_7    ,   KC_8    ,    KC_9   , \
        KC_4    ,   KC_5    ,    KC_6   , \
        KC_1    ,   KC_2    ,    KC_3     \
    )
};
void keyboard_post_init_user(void) {
	  // Customise these values to desired behaviour
    debug_enable=true;
    debug_matrix=true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	  // If console is enabled, it will print the matrix position and status of each key pressed
    #ifdef CONSOLE_ENABLE
        uprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    #endif
    return true;
}
