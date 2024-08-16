//
// Created by Noah on 16.08.2024.
//
#include "Fader.h"

Fader::Fader(int motorPinA, int motorPinB, int potiPin)  :
        motorA(motorPinA, OUTPUT_PWM), motorB(motorPinB, OUTPUT_PWM), poti(potiPin) {

    //getting some starting values
    values[indx++ % 5] = poti.adcReadRaw();
    values[indx++ % 5] = poti.adcReadRaw();
}

