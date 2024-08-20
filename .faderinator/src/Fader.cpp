//
// Created by Noah on 16.08.2024.
//
#include "Fader.h"

Fader::Fader(int motorPinA, int motorPinB, int potiPin) :
        motorA(motorPinA, OUTPUT_PWM), motorB(motorPinB, OUTPUT_PWM), poti(potiPin) {

    //getting some starting values
    for (int i = 0; i < AMOUNT_OF_VALUES / 2; ++i) {
        values[indx++ % AMOUNT_OF_VALUES] = poti.adcReadRaw();
    }
}

void Fader::setTarget(uint16_t newTarget) {
    unsigned int out = (newTarget);
    LOG("Received new target %d\n", out);
    _target = std::clamp(newTarget, _min, _max);
    out = static_cast<std::make_signed_t<uint16_t>>(_target);;
    LOG("Targeting target %d\n", out);
    _moving = true;
}

int32_t Fader::update() {
    for (int i = 0; i < AMOUNT_OF_VALUES / 2; ++i) {
        values[indx++ % AMOUNT_OF_VALUES] = poti.adcReadRaw();
    }
    uint16_t val = 0;
    for (uint16_t el: values) {
        val += el;
    }
    val /= AMOUNT_OF_VALUES;

    if (!_moving) {
        if (dist(val, _target) > _tolerance) {
            //printf("distance: %d between %d, %d\n", dist(val, _target), val, _target);
            _target = val;
            return val;
        }
        return -1;
    }

    unsigned long speed = dist(val, _target);
    speed = speed * speed * ms20 / 5 / 4000;
    if (speed > ms20) speed = ms20;
    if (speed > 300) {
        if (speed < ms20 / 5) speed = ms20 / 5;
        //else if(speed > ms20/4) speed = ms20;
    } else speed = 0;

    if ((_target - val) > _tolerance) {
        motorA.analogWrite(speed, ms20);
        motorB.analogWrite(0, ms20);
        LOG_VERBOSE("%d forward  %lu\n", val, speed);
    } else if ((_target - val) < -_tolerance) {
        motorA.analogWrite(0, ms20);
        motorB.analogWrite(speed, ms20);
        LOG_VERBOSE("%d backward %lu\n", val, speed);
    } else {
        motorA.analogWrite(0, ms20);
        motorB.analogWrite(0, ms20);
        LOG_VERBOSE("%d off\n", val);
        _moving = false;
    }
    return -1;
}