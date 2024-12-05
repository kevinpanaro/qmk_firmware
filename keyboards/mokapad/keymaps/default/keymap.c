// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_driver.h"
#include "quantum.h"
#include "timer.h"
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌─────────────────┐
     * │  o              │
     * ├─────┬─────┬─────┤
     * │  7  │  8  │  9  │
     * ├─────┼─────┼─────┤
     * │  4  │  5  │  6  │
     * ├─────┼─────┼─────┤
     * │  1  │  2  │  3  │
     * └─────┴─────┴─────┘
     */
    [0] = LAYOUT_ortho_3x3_1(
        KC_P0,
        KC_P7,  KC_P8,  KC_P9,
        KC_P4,  KC_P5,  KC_P6,
        KC_P1,  KC_P2,  KC_P3
    )
};

bool attempting_reboot = false;
bool rebooting = false;
bool rebooting_to_boot = false;
uint16_t bootloader_timer = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_P0:
            if (record->event.pressed) {
                if (!attempting_reboot) {
                    attempting_reboot = true;
                }
                bootloader_timer = timer_read();
            } else {
                attempting_reboot = false;
                if (rebooting_to_boot) {
                    reset_keyboard();
                } else if (rebooting) {
                    soft_reset_keyboard();
                }
                oled_clear();
                tap_code(KC_P0);
            }
            return false;
        break;
    }
    return true;
}

void matrix_scan_user(void) { // The very important timer.
    if (attempting_reboot) {
        if (timer_elapsed(bootloader_timer) > 4500) {
            oled_write_P(PSTR("Awaiting New Firmware"), false);
            rebooting_to_boot = true;
        } else if (timer_elapsed(bootloader_timer) > 2500) {
            oled_write_P(PSTR("Rebooting   "), false);
            rebooting = true;
        } else if (timer_elapsed(bootloader_timer) > 2000) {
            oled_write_P(PSTR("Rebooting 1"), false);
        } else if (timer_elapsed(bootloader_timer) > 1500) {
            oled_write_P(PSTR("Rebooting 2"), false);
        } else if (timer_elapsed(bootloader_timer) > 1000) {
            oled_write_P(PSTR("Rebooting 3"), false);
        }
    }
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    if (attempting_reboot) {
        return false;
    }
    // Here is the spacing of the oled.
    //  abcdefghijKlmnopqrstu
    // {---6--|---7---|--6---}

    oled_set_cursor(0, 0);
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_P(PSTR("0"), false);
            break;
        default:
            oled_write_P(PSTR("undefined"), false);
            break;
    }
    return false;
}

void oled_render_boot(bool bootloader) {
    oled_clear();
    if (bootloader) {
        oled_write_P(PSTR("Awaiting New Firmware"), false);
    } else {
        oled_write_P(PSTR("Rebooting"), false);
    }
    oled_render_dirty(true);
}

// Shutdown User : https://docs.qmk.fm/custom_quantum_functions#keyboard-shutdown-reboot-code
bool shutdown_user(bool jump_to_bootloader) {
    oled_render_boot(jump_to_bootloader);
    // false to not process kb level
    return false;
}

#endif
