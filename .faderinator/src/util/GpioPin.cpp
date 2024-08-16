//
// Created by Noah on 16.08.2024.
//
#include "GpioPin.h"
#include <task.h>
#include "VirtualArray.h"


GpioPin::GpioPin(int pin, Mode mode) : pin{pin} {

    switch (mode) {
        case OUTPUT_PWM:
            PADS_BANK0.GPIO[pin].DRIVE = GPIO_DRIVE__4mA;
            VIO_BANK0::GPIO_CTRL[pin]->FUNCSEL = _IO_BANK0_::GPIO_CTRL_FUNCSEL__pwm;
            break;
        case INPUT:
            PADS_BANK0.GPIO[pin].IE = true;
            VIO_BANK0::GPIO_CTRL[pin]->FUNCSEL = _IO_BANK0_::GPIO_CTRL_FUNCSEL__sio;
            PADS_BANK0.GPIO[pin].PUE = 1; //Pullup
            break;
    }

    if (mode == OUTPUT_PWM) {
        VPWM::CH_DIV[pwm_channel(pin)]->INT = div / 16;
        VPWM::CH_DIV[pwm_channel(pin)]->FRAC = div % 16;
    }
}

bool GpioPin::isPressed() {
    for (int i = 0; i < 2; ++i) {
        balance += SIO.GPIO_IN & (1 << pin) ? -1 : 1;
        task::sleep_ms(4);
    }
    if (balance > 4) {
        balance = -1;
        return true;
    }
    return false;
}

void GpioPin::analogWrite(uint16_t cc, uint16_t top) const {
    *VPWM::CH_TOP[pwm_channel(pin)] = top;
    if (pin % 2 == 0)
        VPWM::CH_CC[pwm_channel(pin)]->A = cc;
    else
        VPWM::CH_CC[pwm_channel(pin)]->B = cc;
    PWM_SET.EN = (1 << pwm_channel(pin));
}

int GpioPin::pwm_channel(int pin) {
    return (pin / 2) % 8;
}
