// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku
// generated -*- buffer-read-only: t -*-

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "features/encoder.h"
#include "features/achordion.h"

enum layers { BASE, BUTTON, MEDIA, NAV, MOUSE, SYM, NUM, FUN };

#if defined(SWAP_HANDS_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS] = { 1, 0 };
#endif

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
  // Register encoder clicks
  if (keycode == RT_ENC) {
    tap_code16(C(KC_0));
  } else if (keycode == LT_ENC) {
    tap_code16(KC_MUTE);
  }

  if (!process_achordion(keycode, record)) { return false; }
  // Your macros ...

  return true;
}

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
  [BASE] = { ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) }
}
#endif

void matrix_scan_user(void) {
  achordion_task();
}

// bool get_tapping_force_hold(uint16_t keycode, keyrecord_t* record) {
//   // If you quickly hold a tap-hold key after tapping it, the tap action is
//   // repeated. Key repeating is useful e.g. for Vim navigation keys, but can
//   // lead to missed triggers in fast typing. Here, returning true means we
//   // instead want to "force hold" and disable key repeating.
//   switch (keycode) {
//     // Repeating Z is useful for spamming undo.
//     case HOME_Z:
//       return false;  // Enable key repeating.
//     default:
//       return true;  // Otherwise, force hold and disable key repeating.
//   }
// }

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

void oled_render_layer_state(void) {
  oled_write("Layer: ", false);

  switch (get_highest_layer(layer_state)) {
  case BASE:
    oled_write("Base", false);
    break;
  case BUTTON:
    oled_write("Button", false);
    break;
  case MEDIA:
    oled_write("Media", false);
    break;
  case NAV:
    oled_write("Navigation", false);
    break;
  case MOUSE:
    oled_write("Mouse", false);
    break;
  case SYM:
    oled_write("Symbol", false);
    break;
  case NUM:
    oled_write("Number", false);
    break;
  case FUN:
    oled_write("Function", false);
    break;
  }

  oled_write("\n", false);
}

void oled_render_led_state(void) {
  led_t led_state = host_keyboard_led_state();

  oled_write(led_state.caps_lock ? "CAPS " : "      ", false);
  oled_write(led_state.scroll_lock ? "SCROLL " : "       ", false);
  oled_write("\n", false);
}

void oled_render_mod_state(void) {
  uint8_t mod_state = get_mods();

  oled_write((mod_state & MOD_MASK_SHIFT) ? "SHIFT " : "      ", false);
  oled_write((mod_state & MOD_MASK_CTRL) ? "CTRL " : "     ", false);
  oled_write((mod_state & MOD_MASK_ALT) ? "ALT " : "    ", false);
  oled_write((mod_state & MOD_MASK_GUI) ? "GUI" : "   ", false);
  oled_write("\n", false);
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
    oled_render_mod_state();
    oled_render_led_state();
  } else {
    oled_render_logo();
  }
  return false;
}

#endif // OLED_ENABLE