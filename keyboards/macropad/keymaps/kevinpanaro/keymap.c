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
    uint8_t pixel_index = 0;
#endif

// Defines names for use in layer keycodes and the keymap
// if you add a layer here, make sure to update the num_of_layers definition
enum layer_names {
    _BASE=0,
    _DISCORD=1,
    _VALORANT=2,
    _NUMPAD=3,
    _HASS=4,
    _COD=5,
    _SPOTIFY=6,
};

#define num_of_layers 7

enum custom_keycodes {
    MOVE_HOME = SAFE_RANGE,
    VAL_YES,
    VAL_NO,
    VAL_ULT,
    VAL_COMMEND,
    VAL_VOTE_YES,
    VAL_VOTE_NO,
    VAL_NO_TIME,
};

// Tap Dance Declarations
enum {
    TD_1_0,
};

// Combos
enum combo_events {
    COMBO_RESET,
};
// COMBO; update these with the bottom right and top left of _BASE layer
const uint16_t PROGMEM reset_combo[] = {KC_I, KC_A, KC_NO, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [COMBO_RESET] = COMBO_ACTION(reset_combo),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
        KC_NO,
        KC_A    ,   KC_B    ,    KC_C   ,
        KC_D    ,   KC_E    ,    KC_F   ,
        KC_G    ,   KC_H    ,    KC_I
    ),
    [_DISCORD] = LAYOUT(
        MOVE_HOME,
	      MEH(KC_F1),   KC_T    ,    KC_U   ,
        KC_V    ,   KC_W    ,    KC_X   ,
	      KC_Y    ,   KC_Z    ,    KC_R
    ),
    [_VALORANT] = LAYOUT(
        MOVE_HOME,
        MEH(KC_F1),   KC_T    ,    KC_U   ,
        VAL_NO_TIME ,   KC_W    ,    VAL_COMMEND  ,
        VAL_ULT ,  VAL_NO   ,    VAL_YES
    ),
    [_NUMPAD] = LAYOUT(
        MOVE_HOME,
        KC_7    ,   KC_8    ,    KC_9   ,
        KC_4    ,   KC_5    ,    KC_6   ,
        TD_1_0    ,   KC_2    ,    KC_3
    ),
    [_HASS] = LAYOUT(
        MOVE_HOME,
        KC_NO   ,   KC_NO   ,    KC_NO  ,
        KC_NO   ,   KC_NO   ,    KC_NO  ,
        KC_F19  ,   KC_NO   ,    KC_NO
    ),
    [_COD] = LAYOUT(
        MOVE_HOME,
        MEH(KC_F1),   KC_F8   ,    KC_F7  ,
        KC_NO   ,   KC_NO   ,    KC_NO  ,
        KC_NO   ,   KC_NO   ,    KC_F10
    ),
    [_SPOTIFY] = LAYOUT(
        MOVE_HOME,
	      A(KC_F20),    KC_F20, C(KC_F20),
        SFT_T(KC_NO), KC_NO ,  KC_NO,
        KC_NO    ,    KC_NO ,  KC_NO
    )
};


qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_1_0] = ACTION_TAP_DANCE_DOUBLE(1, 0),
};

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch(combo_index) {
        case COMBO_RESET:
            if (pressed) {
		            tap_code16(RESET);
	          }
	          break;
	  }
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    switch (keycode) {
	      case MOVE_HOME:
	          if (record->event.pressed) {
		            layer_move(0);
	          } else {
	          }
	          return false;
        case VAL_YES:
	          if (record->event.pressed) {
	              SEND_STRING(".33");
	          } else {
	          }
	          return false;
	      case VAL_NO:
	          if (record->event.pressed) {
	              SEND_STRING(".34");
	          } else {
	          }
	          return false;
	      case VAL_ULT:
	          if (record->event.pressed) {
	              SEND_STRING(".15");
	          } else {
	          }
	          return false;
	      case VAL_COMMEND:
	          if (record->event.pressed) {
	              SEND_STRING(".32");
	          } else {
	          }
	          return false;
        case VAL_NO_TIME:
	          if (record->event.pressed) {
                register_code(KC_LSHIFT);
                tap_code(KC_ENT);
                unregister_code(KC_LSHIFT);
	              SEND_STRING("NO TIME");
                tap_code(KC_ENT);
	          } else {
	          }
	          return false;
	      default:
	          return true;
    }
}

void next_layer(void) {
    uint8_t layer = get_highest_layer(layer_state);
    if ( ( layer + 1 ) < num_of_layers ) {
        layer = layer + 1;
    } else {
        layer = 0;
    }
    layer_move(layer);
}

void prev_layer(void) {
    uint8_t layer = get_highest_layer(layer_state);
    if ( layer == 0 ) {
	      layer = num_of_layers - 1;
    } else {
	      layer = layer - 1;
    }
    layer_move(layer);
}

void encoder_update_user(uint8_t index, bool clockwise) {
    if ( clockwise ) {
	      next_layer();
    } else {
	      prev_layer();
    }
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
     oled_write_P(PSTR("layer: "), false);

     switch (get_highest_layer(layer_state)) {
    	 case _BASE:
    	     oled_write_P(PSTR("default\n"), false);
    	     break;
    	 case _DISCORD:
    	     oled_write_P(PSTR("discord\n"), false);
    	     break;
    	 case _VALORANT:
    	     oled_write_P(PSTR("valorant\n"), false);
    	     break;
    	 case _NUMPAD:
    	     oled_write_P(PSTR("numpad\n"), false);
    	     break;
    	 case _HASS:
    	     oled_write_P(PSTR("home assistant\n"), false);
    	     break;
    	 case _COD:
    	     oled_write_P(PSTR("call of duty\n"), false);
    	     break;
    	 case _SPOTIFY:
    	     oled_write_P(PSTR("spotify\n"), false);
    	     break;
    	 default:
    	     oled_write_P(PSTR("undefined\n"), false);
    	     break;
     }
     // Host Keyboard LED Status
     led_t led_state = host_keyboard_led_state();
     oled_write_P(led_state.caps_lock ? PSTR(" caps ") : PSTR(""), false);

}
#endif

#ifdef RAW_ENABLE
void raw_hid_receive(uint8_t *data, uint8_t length) {
    //oled_scroll_set_area(start_line, end_line);
    // if we are here, the pc is connected
    //is_hid_connected = true;
    const char *oled_data = (char*)data;

    switch( data[0] ) {
        case 1:
            oled_set_cursor(0, data[1]);
            oled_write(oled_data + 2, false);
            break;
        case 2:
            pixel_index = 1;
            while(pixel_index < RAW_EPSIZE && data[pixel_index] != 0xff){
                oled_write_pixel(data[pixel_index], data[pixel_index + 1], true);
                pixel_index += 2;
            }
            break;
        case 3:
            switch( data[1] ) {
                case 0:
                    oled_scroll_off();
                    break;
                case 1:
                    oled_scroll_left();
                    break;
                case 2:
                    oled_scroll_right();
                    break;
                case 3:
                    oled_scroll_left();
                    break;
                case 4:
                    oled_scroll_set_speed(data[2]);
                    break;
                case 5:
                    oled_scroll_set_area(data[2], data[3]);
                    break;
                default:
                    break;
            }
        case 8:
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
}
#endif
