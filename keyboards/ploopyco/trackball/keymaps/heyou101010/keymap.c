/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
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
#include "keycode.h"
// #include "pointing_device.h"
#include "trackball.h"

enum layers {
    _BASE, // default layer
    _EDIT, // copy/cut/paste
    _SCRL, // scrolling modifiers
    _VLME, // scroll volume, mute
    _XTRA, // bootloader, etc
};

enum custom_keycodes { COPY = PLOOPY_SAFE_RANGE, CUT, PASTE, MAC_MODE };
static bool is_mac_mode = false;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_BTN1,
        KC_BTN3,
        LT(_XTRA,KC_BTN4),   /* . = KC_BTN4  | _ = _XTRA layer  */
        LT(_EDIT,KC_BTN2),   /* . = KC_BTN2  | _ = _EDIT layer  */
        LT(_SCRL,KC_BTN5)    /* . = KC_BTN5  | _ = _SCRL layer  */
    ),
    [_EDIT] = LAYOUT(
        COPY,
        CUT,
        PASTE,
        _______,        /* Being held to be on this layer */
        LT(_SCRL,KC_ENT)         /* Leaving transparent so _SCRL can activate */
    ),
    [_SCRL] = LAYOUT(
        DRAG_SCROLL,
        DPI_CONFIG,
        MAC_MODE,
        _______,        /* Leaving transparent so _EDIT can activate */
        _______         /* Being held to be on this layer */
    ),
    [_VLME] = LAYOUT(
        G(S(KC_A)),     /* Mute Mic via PowerToys */
        KC_MUTE,        /* Mute speaker audio */
        G(S(KC_O)),     /* Disable camera via PowerToys */
        _______,        /* Being held to be on this layer */
        _______         /* Being held to be on this layer */
    ),
    [_XTRA] = LAYOUT(
        _______,
        QK_BOOTLOADER,
        _______,        /* Being held to be on this layer */
        _______,
        _______
    )
};

// clang-format on

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (IS_LAYER_ON(_VLME)) {
        clockwise ? tap_code16(KC_VOLU) : tap_code16(KC_VOLD);
        return false;
    }

    if (IS_LAYER_ON(_SCRL)) {
        clockwise ? tap_code16(KC_MS_WH_LEFT) : tap_code16(KC_MS_WH_RIGHT);
        return false;
    }

    if (is_mac_mode){
        clockwise ? tap_code16(KC_MS_WH_DOWN) : tap_code16(KC_MS_WH_UP);
        return false;
    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    set_single_persistent_default_layer(_BASE);
    layer_state_t new_state = update_tri_layer_state(state, _EDIT, _SCRL, _VLME);

    return new_state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case COPY:
            if (record->event.pressed) {
                tap_code16(C(KC_C));
            }
            return false;
        case CUT:
            if (record->event.pressed) {
                tap_code16(C(KC_X));
            }
            return false;
        case PASTE:
            if (record->event.pressed) {
                tap_code16(C(KC_V));
            }
            return false;
        case MAC_MODE:
            if (record->event.pressed) {
                is_mac_mode = !is_mac_mode;
                keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = is_mac_mode;

            }
            return false;

        default:
            break;
    }

    return true;
}
