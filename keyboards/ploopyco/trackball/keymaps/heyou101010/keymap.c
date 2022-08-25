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
#include "pointing_device.h"
#include "print.h"
#include "trackball.h"

enum layers {
    _BASE, // default layer
    _EDIT, // copy/cut/paste
    _SCRL, // scrolling modifiers
    _VLME, // scroll volume, mute
    _XTRA, // bootloader, etc
};

enum custom_keycodes { COPY = PLOOPY_SAFE_RANGE, CUT, PASTE };

enum td_keycodes { BTN_EDIT, BTN_SCRL, BTN_XTRA };

typedef enum { TD_NONE, TD_UNKNOWN, TD_TAP, TD_HOLD, TD_DOUBLE_TAP } td_state_t;

// tapdance function declarations
td_state_t cur_dance(qk_tap_dance_state_t *state);

void btn_edit_finished(qk_tap_dance_state_t *state, void *user_data);
void btn_edit_reset(qk_tap_dance_state_t *state, void *user_data);
void btn_scrl_finished(qk_tap_dance_state_t *state, void *user_data);
void btn_scrl_reset(qk_tap_dance_state_t *state, void *user_data);
void btn_xtra_finished(qk_tap_dance_state_t *state, void *user_data);
void btn_xtra_reset(qk_tap_dance_state_t *state, void *user_data);

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_BTN1,
        KC_BTN3,
        TD(BTN_XTRA),   /* . = KC_BTN4  | _ = _XTRA layer  */
        TD(BTN_EDIT),   /* . = KC_BTN2  | _ = _EDIT layer  */
        TD(BTN_SCRL)    /* . = KC_BTN5  | _ = _SCRL layer  */
    ),
    [_EDIT] = LAYOUT(
        COPY,
        CUT,
        PASTE,
        _______,        /* Being held to be on this layer */
        _______         /* Leaving transparent so _SCRL can activate */
    ),
    [_SCRL] = LAYOUT(
        DRAG_SCROLL,
        DPI_CONFIG,
        _______,
        _______,        /* Leaving transparent so _EDIT can activate */
        _______         /* Being held to be on this layer */
    ),
    [_VLME] = LAYOUT(
        G(C(KC_A)),     /* Mute Mic via PowerToys */
        KC_MUTE,        /* Mute speaker audio */
        G(C(KC_O)),     /* Disable camera via PowerToys */
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
#ifdef CONSOLE_ENABLE
        uprintf("_VLME layer enabled, and clockwise = %d\n", clockwise);
#endif
        if (clockwise) {
            tap_code16(KC_VOLU);
        } else {
            tap_code16(KC_VOLD);
        }
        return false;
    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    set_single_persistent_default_layer(_BASE);
    layer_state_t new_state = update_tri_layer_state(state, _EDIT, _SCRL, _VLME);

#ifdef CONSOLE_ENABLE
    if (layer_state_cmp(new_state, _EDIT)) {
        uprintln("_EDIT layer is on");
    } else {
        uprintln("_EDIT layer is off");
    }
    if (layer_state_cmp(new_state, _SCRL)) {
        uprintln("_SCRL layer is on");
    } else {
        uprintln("_SCRL layer is off");
    }
    if (layer_state_cmp(new_state, _VLME)) {
        uprintln("_VLME layer is on");
    } else {
        uprintln("_VLME layer is off");
    }
    if (layer_state_cmp(new_state, _XTRA)) {
        uprintln("_XTRA layer is on");
    } else {
        uprintln("_XTRA layer is off");
    }
    if (layer_state_cmp(new_state, _BASE)) {
        uprintln("_BASE layer is on");
    } else {
        uprintln("_BASE layer is off");
    }
#endif

    return new_state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case COPY:
            if (record->event.pressed) {
                tap_code16(C(KC_C));
            }
            return false;
            break;
        case CUT:
            if (record->event.pressed) {
                tap_code16(C(KC_X));
            }
            return false;
            break;
        case PASTE:
            if (record->event.pressed) {
                tap_code16(C(KC_V));
            }
            return false;
            break;

        default:
            break;
    }

    return true;
}

// tap dance functions
td_state_t cur_dance(qk_tap_dance_state_t *state) {
#ifdef CONSOLE_ENABLE
    uprintf("current dance state: count = %d | pressed = %d | finished = %d \r\n", state->count, state->pressed, state->finished);
#endif
    switch (state->count) {
        case 1:
            if (!state->pressed) {
                return TD_TAP;
            } else {
                return TD_HOLD;
            }
        case 2:
            return TD_DOUBLE_TAP;
        default:
            return TD_UNKNOWN;
    }
}

static td_state_t td_state;

void btn_edit_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);

    // clang-format off
    switch (td_state) {
        case TD_TAP: tap_code16(KC_BTN2); break;
        case TD_HOLD: layer_on(_EDIT); break;
        default: break;
    }
    // clang-format on
}

void btn_edit_reset(qk_tap_dance_state_t *state, void *user_data) {
    layer_off(_EDIT);
}

void btn_scrl_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);

    // clang-format off
    switch (td_state) {
        case TD_TAP: tap_code16(KC_BTN5); break;
        case TD_HOLD: layer_on(_SCRL); break;
        default: break;
    }
    // clang-format on
}

void btn_scrl_reset(qk_tap_dance_state_t *state, void *user_data) {
    layer_off(_SCRL);
}

void btn_xtra_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    // clang-format off
    switch (td_state) {
        case TD_TAP: tap_code16(KC_BTN4); break;
        case TD_HOLD: layer_on(_XTRA); break;
        default: break;
    }
    // clang-format on
}

void btn_xtra_reset(qk_tap_dance_state_t *state, void *user_data) {
    layer_off(_XTRA);
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [BTN_EDIT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, btn_edit_finished, btn_edit_reset),
    [BTN_SCRL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, btn_scrl_finished, btn_scrl_reset),
    [BTN_XTRA] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, btn_xtra_finished, btn_xtra_reset),
};
