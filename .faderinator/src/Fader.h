//
// Created by Noah on 09.08.2024.
//

#ifndef FADERINATOR_FADER_H
#define FADERINATOR_FADER_H

#include <adc_rp2040.h>
#include <task.h>
#include <cstdio>
#include "util/GpioPin.h"
#include "io/SerialConnection.h"

class Fader {
    static constexpr int AMOUNT_OF_VALUES = 10;

    GpioPin motorA, motorB;
    adc_rp2040_channel poti;
    uint16_t _target = 0, _tolerance = 15;
    uint16_t _min = 8, _offset = 0, _max = 4057;
    bool _moving{};

    uint16_t values[AMOUNT_OF_VALUES];
    int indx = 0;

public:
    Fader(int motorPinA, int motorPinB, int potiPin);

    void setTarget(uint16_t newTarget);

    int32_t update();

    [[nodiscard]] static uint16_t dist(uint16_t a, uint16_t b) {
        return b > a ? (b - a) : (a - b);
    }
};

#endif //FADERINATOR_FADER_H
