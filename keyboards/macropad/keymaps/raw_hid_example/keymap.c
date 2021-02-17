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

#ifdef RAW_ENABLE
    #include "raw_hid.h"
    #include <string.h>
    #define RAW_EPSIZE 64
#endif

// Defines names for use in layer keycodes and the keymap
// if you add a layer here, make sure to update the num_of_layers definition
enum layer_names {
    _BASE=0,
};

#define num_of_layers = Enum.GetNames(typeof(layer_names)).Length

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(                     \
        KC_0    ,                         \
        KC_7    ,   KC_8    ,    KC_9   , \
        KC_4    ,   KC_5    ,    KC_6   , \
        KC_1    ,   KC_2    ,    KC_3     \
    )
};

bool is_hid_connected = false; // is pc connected yet?
uint8_t screen_max_count = 0;  // number of info screens we can scroll through
uint8_t screen_show_index = 0; // current index of the info screens
uint8_t screen_data_buffer[64 - 1] =  {0}; // buffer for screen data
int screen_data_index = 0;     // current index into screen data buffer
uint8_t start_line = 0;
uint8_t end_line = 0;

#ifdef OLED_DRIVER_ENABLE
// void oled_task_user(void) {
//     // Host Keyboard LED Status
//     led_t led_state = host_keyboard_led_state();
//     oled_write_P(led_state.caps_lock ? PSTR("caps ") : PSTR(" "), false);
// }
#endif

#ifdef RAW_ENABLE
void raw_hid_receive(uint8_t *data, uint8_t length) {
    //oled_scroll_set_area(start_line, end_line);
    // if we are here, the pc is connected
    is_hid_connected = true;
    const char *oled_data = (char*)data;

    switch( data[0] ) {
        case 8:
            oled_clear();
            break;
        default:
            oled_set_cursor(0, data[0]);
            oled_write(oled_data + 1, false);
        }
    //raw_hid_send(data, length);
}
#endif
