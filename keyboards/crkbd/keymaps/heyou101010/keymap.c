/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>

char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

enum custom_keys {
    LFT_ENC = SAFE_RANGE,
    RT_ENC
    ARR_FN 
};

enum layers {
    _BASE,
    _NUM,
    _SYM,
    _FN,
    _NAV,
    _SYS
};

// TAP_HOLD FROM DOCS
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

enum tap_dances {
    CBR_HDNC,
    SQBR_HDNC,
    PRN_HDNC,
    COLN_HDNC,
    SLSH_HDNC,
    DOT_HDNC,
    QUOT_HDNC,
    ANGB_HDNC,
    QUES_HDNC,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        set_keylog(keycode, record);
    }

    qk_tap_dance_action_t *action;

    switch (keycode) {
        case TD(CBR_HDNC):
        case TD(SQBR_HDNC):
        case TD(PRN_HDNC):
        case TD(COLN_HDNC):
        case TD(SLSH_HDNC):
        case TD(DOT_HDNC):
        case TD(QUOT_HDNC):
        case TD(ANGB_HDNC):
        case TD(QUES_HDNC):  // list all tap dance keycodes with tap-hold configurations
            action = &tap_dance_actions[TD_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

void tap_dance_tap_hold_finished(qk_tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(qk_tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

// END TAP_HOLD FROM DOCS

qk_tap_dance_action_t tap_dance_actions[] = {
    [CBR_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_LCBR, KC_RCBR),
    [SQBR_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_LBRC, KC_RBRC),
    [PRN_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_LPRN, KC_RPRN),
    [COLN_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_COLN, KC_SCLN),
    [SLSH_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_SLSH, KC_BSLS),
    [DOT_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_DOT, KC_COMM),
    [QUOT_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_QUOT, KC_DQUO),
    [ANGB_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_LT, KC_GT),
    [QUES_HDNC] = ACTION_TAP_DANCE_TAP_HOLD(KC_QUES, KC_EXLM),
};

#define GUI_ESC LGUI_T(KC_ESC)
#define CTL_SP LCTL_T(KC_SPC)

#define OS_MEH OSM(MOD_MEH)
#define OS_SFT OSM(MOD_LSFT)
#define OS_ALT OSM(MOD_LALT)
#define OS_CTL OSM(MOD_LCTL)
#define OS_GUI OSM(MOD_LCTL)

#define L_BASE TO(_BASE)
#define L_NUM TO(_NUM)
#define L_SYM TO(_SYM)
#define L_FN TO(_FN)
#define L_NAV TO(_NAV)
#define L_SYS TO(_SYS)

#define TD_CBR TD(CBR_HDNC)
#define TD_SQBR TD(SQBR_HDNC)
#define TD_PRN TD(PRN_HDNC)
#define TD_SLSH TD(SLSH_HDNC)
#define TD_COLN TD(COLN_HDNC)
#define TD_DOT TD(DOT_HDNC)
#define TD_QUOT TD(QUOT_HDNC)
#define TD_ANGB TD(ANGB_HDNC)
#define TD_QUES TD(QUES_HDNC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      LFT_ENC,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                         KC_J,    KC_L,    KC_U,    KC_Y,    KC_P,  RT_ENC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TAB,     KC_A,    KC_R,    KC_S,    KC_T,    KC_G,                         KC_M,    KC_N,    KC_E,    KC_I,    KC_O, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_ESC,     KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                         KC_K,    KC_H, TD_QUES,  TD_DOT, TD_SLSH,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           OS_GUI,  OS_ALT,  KC_SPC,     KC_ENT,   L_NUM,   OS_CTL
                                      //`--------------------------'  `--------------------------'

  ),

  [_NUM] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      LFT_ENC,   L_SYS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                       KC_EQL,    KC_7,    KC_8,    KC_9, KC_MINS,  RT_ENC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TAB,   OS_GUI,  OS_ALT,  OS_SFT,  OS_CTL, XXXXXXX,                         KC_0,    KC_4,    KC_5,    KC_6, KC_ASTR, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_ESC,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                       TD_DOT,    KC_1,    KC_2,    KC_3, TD_SLSH,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          L_BASE,    L_NAV,  KC_SPC,     KC_ENT,   L_SYM,    L_FN
                                      //`--------------------------'  `--------------------------'

  ),

  [_SYM] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      LFT_ENC,   L_SYS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                       TD_CBR, TD_ANGB, KC_EXLM,   KC_AT, KC_HASH,  RT_ENC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TAB,   OS_GUI,  OS_ALT,  OS_SFT,  OS_CTL, XXXXXXX,                       TD_PRN, KC_AMPR, KC_PIPE,  KC_GRV, TD_QUOT, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_ESC,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      TD_SQBR, KC_UNDS,  KC_GRV, TD_COLN, KC_SLSH,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          L_BASE,    L_NAV,  KC_SPC,     KC_ENT,   L_NUM,    L_FN
                                      //`--------------------------'  `--------------------------'

  ),

  [_FN] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      LFT_ENC,   L_SYS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MNXT,   KC_F1,   KC_F2,   KC_F3,   KC_F4,  RT_ENC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TAB,   OS_GUI,  OS_ALT,  OS_SFT,  OS_CTL, XXXXXXX,                      KC_MPLY,   KC_F5,   KC_F6,   KC_F7,   KC_F8, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_ESC,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MPRV,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           L_BASE,   L_NAV,  KC_SPC,     KC_ENT,   L_NUM,  XXXXXXX
                                      //`--------------------------'  `--------------------------'
  ),

  [_NAV] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      LFT_ENC,   L_SYS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                       KC_APP, KC_HOME,   KC_UP,  KC_END, KC_PGUP,  RT_ENC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TAB,   OS_GUI,  OS_ALT,  OS_SFT,  OS_CTL, XXXXXXX,                      XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_ESC,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           L_BASE,   L_NAV,  KC_SPC,     L_SYS,    L_NUM,    L_FN
                                      //`--------------------------'  `--------------------------'
  ),

  [_SYS] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           L_BASE,   L_NAV,  KC_SPC,     L_SYS,    L_NUM,    L_FN
                                      //`--------------------------'  `--------------------------'
  )
};

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case _NUM:
            oled_write_ln_P(PSTR("Number"), false);
            break;
        case _SYM:
            oled_write_ln_P(PSTR("Symbol"), false);
            break;
        case _FN:
            oled_write_ln_P(PSTR("Function"), false);
            break;
        case _NAV:
            oled_write_ln_P(PSTR("Navigation"), false);
            break;
        case _SYS:
            oled_write_ln_P(PSTR("System"), false);
            break;
    }
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}
