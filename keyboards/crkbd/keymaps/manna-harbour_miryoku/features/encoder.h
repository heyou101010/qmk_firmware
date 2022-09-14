#pragma once

// #include "gpio.h"
#define ENCODERS_PAD_A { B4 }
#define ENCODERS_PAD_B { B5 }
// #define ENCODER_RESOLUTIONS { 4 }
// #define ENCODER_RESOLUTIONS_RIGHT { 4 }

#include "quantum_keycodes.h"

enum encoders {
  LT_ENC = SAFE_RANGE,
  RT_ENC,
  CUSTOM_SAFE_RANGE
};