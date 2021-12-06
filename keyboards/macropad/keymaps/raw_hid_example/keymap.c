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
    #define RAW_EPSIZE 32
#endif

// Defines names for use in layer keycodes and the keymap
// if you add a layer here, make sure to update the num_of_layers definition
enum layer_names {
    _BASE=0,
    _TEST=1,
};

enum raw_hid_commands {
    WRITE=1,
    PIXEL=2,
    SCROLL=3,
    BRIGHTNESS=4,
    QUERY=5,
    CLEAR=8,
};

#define num_of_layers = Enum.GetNames(typeof(layer_names)).Length

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(                     \
        KC_0    ,                         \
        KC_7    ,   KC_8    ,    KC_9   , \
        KC_4    ,   KC_5    ,    KC_6   , \
        KC_1    ,   KC_2    ,    TO(_TEST)     \
    ),
    [_TEST] = LAYOUT(                     \
        RESET   ,                         \
        KC_7    ,   KC_8    ,    KC_9   , \
        KC_4    ,   KC_5    ,    KC_6   , \
        KC_1    ,   TO(_BASE)    ,    KC_3     \
    )
};

bool is_hid_connected = false; // is pc connected yet?
uint8_t screen_max_count = 0;  // number of info screens we can scroll through
uint8_t screen_show_index = 0; // current index of the info screens
uint8_t screen_data_buffer[64 - 1] =  {0}; // buffer for screen data
int screen_data_index = 0;     // current index into screen data buffer
uint8_t start_line = 0;
uint8_t end_line = 0;
uint8_t pixel_index = 0;
bool pixel_state = true;

void raw_hid_send_current_layer(uint8_t layer) {
    if (is_hid_connected) {
        uint8_t send_data[RAW_EPSIZE] = {0};
        send_data[0] = layer;
        raw_hid_send(send_data, sizeof(send_data));
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t current_layer = get_highest_layer(state);
    raw_hid_send_current_layer(current_layer);
    switch(current_layer) {
        case _BASE:
            break;
        case _TEST:
            break;
        default:
            break;
    }
    return state;
}


#ifdef OLED_ENABLE
bool oled_task_user(void) {
    oled_write_P(PSTR("layer: "), false);


    switch (get_highest_layer(layer_state)) {
    	case _BASE:
    	    oled_write_P(PSTR("0\n"), false);
    	    break;
    	case _TEST:
    	    oled_write_P(PSTR("1\n"), false);
    	    break;
        default:
            break;
    }
    return false;
}
#endif

#ifdef RAW_ENABLE
void raw_hid_receive(uint8_t *data, uint8_t length) {
    //oled_scroll_set_area(start_line, end_line);
    // if we are here, the pc is connected
    is_hid_connected = true;
    const char *oled_data = (char*)data;
    uint8_t send_data[RAW_EPSIZE] = {0};
    uint8_t command = data[0];

    switch( command ) {
        case WRITE:
            oled_set_cursor(0, data[1]);
            oled_write(oled_data + 2, false);
            break;
        case PIXEL:
            if ( data[1] ) {
                pixel_state = true;
            } else {
                pixel_state = false;
            }
            pixel_index = 2;
            while(pixel_index < RAW_EPSIZE && data[pixel_index] != 0xff){
                oled_write_pixel(data[pixel_index], data[pixel_index + 1], pixel_state);
                pixel_index += 2;
            }
            break;
        case SCROLL:
        // this section is so buggy, please use at own risk
            switch( data[1] ) {
                case 1:
                    oled_scroll_off();
                    break;
                case 2:
                    oled_scroll_left();
                    break;
                case 3:
                    oled_scroll_right();
                    break;
                case 4:
                    oled_scroll_left();
                    break;
                case 5:
                    oled_scroll_set_speed(data[2]);
                    break;
                case 6:
                    oled_scroll_set_area(data[2], data[3]);
                    break;
                default:
                    break;
            }
            break;
        case BRIGHTNESS:
            oled_set_brightness(data[1]);
            break;
        case QUERY:
            switch( data[1] ) {
                case 1:
                    if ( is_oled_on() ) {
                        send_data[0] = 1;
                    } else {
                        send_data[0] = 0;
                    }
                    raw_hid_send(send_data, length);
                    break;
                case 2:
                    oled_on();
                    break;
                case 3:
                    oled_off();
                    break;
                case 4:
                    // current layer
                    send_data[0] = get_highest_layer(layer_state);
                    raw_hid_send(send_data, length);
                case 5:
                    // current brightness
                    send_data[0] = oled_get_brightness();
                    raw_hid_send(send_data, length);
                    break;
                case 6:
                    send_data[0] = oled_max_chars();
                    raw_hid_send(send_data, length);
                    break;
                case 7:
                    send_data[0] = oled_max_lines();
                    raw_hid_send(send_data, length);
                    break;
                default:
                    break;
            }
            break;
        case CLEAR:
            switch( data[1] ) {
                case 8:
                    oled_clear();
                    break;
                default:
                    oled_set_cursor(0, data[1]);
                    oled_advance_page(true);
                    break;
            }
            break;
        default:
            break;
        }
    //raw_hid_send(data, length);
}
#endif
