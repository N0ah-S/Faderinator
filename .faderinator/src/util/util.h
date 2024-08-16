#ifndef FADERINATOR_UTIL_H
#define FADERINATOR_UTIL_H

#define WS2812_PIN 29   // The GPIO pin controlling all the LEDs
#define LED_COUNT  8    // Number of LEDs
#define BUZZER 14
#define S1  16    // S1 / left btn
#define S2  17    // S2 / right btn

#define ms20 65535
#define micro (ms20/20/1000)

#include "ws2812_rp2040.h"

typedef ws2812_rp2040 LedArray;

typedef uint8_t byte;

using namespace _PADS_BANK0_;
using namespace _IO_BANK0_;
using namespace _SIO_;
using namespace _PWM_;

#endif //FADERINATOR_UTIL_H