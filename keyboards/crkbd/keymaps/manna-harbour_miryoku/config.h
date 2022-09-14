// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "features/encoder.h"

#define LAYOUT_miryoku(\
        K00,   K01,   K02,   K03,   K04,          K05,   K06,   K07,   K08,   K09,\
        K10,   K11,   K12,   K13,   K14,          K15,   K16,   K17,   K18,   K19,\
        K20,   K21,   K22,   K23,   K24,          K25,   K26,   K27,   K28,   K29,\
        N30,   N31,   K32,   K33,   K34,          K35,   K36,   K37,   N38,   N39\
)\
LAYOUT_split_3x6_3(\
LT_ENC, K00,   K01,   K02,   K03,   K04,          K05,   K06,   K07,   K08,   K09,  RT_ENC,\
KC_NO,  K10,   K11,   K12,   K13,   K14,          K15,   K16,   K17,   K18,   K19,   KC_NO,\
KC_NO,  K20,   K21,   K22,   K23,   K24,          K25,   K26,   K27,   K28,   K29,   KC_NO,\
                      K32,   K33,   K34,          K35,   K36,   K37\
)

//#define USE_MATRIX_I2C
#ifdef KEYBOARD_crkbd_rev1_common
#    undef USE_I2C
#    define USE_SERIAL
#endif

/* Select hand configuration */

#define MASTER_LEFT
// #define MASTER_RIGHT
// #define EE_HANDS

#define SSD1306OLED
#define USE_SERIAL_PD2

// #undef RGBLED_NUM
// #define RGBLIGHT_ANIMATIONS
// #define RGBLED_NUM 27
// #define RGBLIGHT_LIMIT_VAL 120
// #define RGBLIGHT_HUE_STEP 10
// #define RGBLIGHT_SAT_STEP 17
// #define RGBLIGHT_VAL_STEP 17

#ifdef OLED_FONT_H
       #undef OLED_FONT_H
#endif
#define OLED_FONT_H "keyboards/crkbd/lib/glcdfont.c"

// Tap-hold configuration for home row mods.
#define IGNORE_MOD_TAP_INTERRUPT
#define PERMISSIVE_HOLD
#define TAPPING_FORCE_HOLD_PER_KEY

#ifdef TAPPING_TERM
       #undef TAPPING_TERM
#endif
#define TAPPING_TERM 180