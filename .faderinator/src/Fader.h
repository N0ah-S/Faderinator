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
    GpioPin motorA, motorB;
    adc_rp2040_channel poti;
    uint16_t _target = 0, _tolerance = 15;
    uint16_t _min = 8, _offset = 0, _max = 4057;
    bool _moving{};

    uint16_t values[10];
    int indx = 0;

public:
    Fader(int motorPinA, int motorPinB, int potiPin);

    void setTarget(uint16_t newTarget) {
        unsigned int out = (newTarget);
        SerialConnection::inst->print("Received new target %d\n", out);
        _target = std::clamp(newTarget, _min, _max);
        out = static_cast<std::make_signed_t<uint16_t>>(_target);;
        SerialConnection::inst->print("Targeting target %d\n", out);
        _moving = true;
    }

    int32_t update() {
        values[indx++ % 10] = poti.adcReadRaw();
        values[indx++ % 10] = poti.adcReadRaw();
        values[indx++ % 10] = poti.adcReadRaw();
        values[indx++ % 10] = poti.adcReadRaw();
        values[indx++ % 10] = poti.adcReadRaw();
        values[indx++ % 10] = poti.adcReadRaw();
        values[indx++ % 10] = poti.adcReadRaw();
        uint16_t val = (values[0] + values[1] + values[2] + values[3] + values[4] + values[5] + values[6] + values[7] + values[8] + values[9]) / 10;

        if(!_moving) {
            if(dist(val, _target) > _tolerance) {
                //printf("distance: %d between %d, %d\n", dist(val, _target), val, _target);
                _target = val;
                return val;
            }
            return -1;
        }

        unsigned long speed = dist(val, _target);
        speed = speed * speed * ms20 / 5 / 4000;
        if(speed > ms20) speed = ms20;
        if(speed > 300) {
            if(speed < ms20/5) speed = ms20/5;
            //else if(speed > ms20/4) speed = ms20;
        } else speed = 0;

        if((_target - val) > _tolerance) {
            motorA.analogWrite(speed, ms20);
            motorB.analogWrite(0, ms20);
            printf("%d forward  %lu\n", val, speed);
        } else if((_target - val) < -_tolerance) {
            motorA.analogWrite(0, ms20);
            motorB.analogWrite(speed, ms20);
            printf("%d backward %lu\n", val, speed);
        } else {
            motorA.analogWrite(0, ms20);
            motorB.analogWrite(0, ms20);
            printf("%d off\n", val);
            _moving = false;
        }
        return -1;
    }

    [[nodiscard]] static uint16_t dist(uint16_t a, uint16_t b) {
        return b > a ? (b - a) : (a - b);
    }
};

#endif //FADERINATOR_FADER_H
