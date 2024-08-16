//
// Created by Noah on 25.04.2024.
//
#ifndef FADERINATOR_RGBLED_H
#define FADERINATOR_RGBLED_H


#include <cstdint>

enum Mode {
    OUTPUT_PWM , INPUT
};

class GpioPin {
    static constexpr uint32_t div = 125000000 / 50 / 4096;
    int pin;
    int balance = 0;
public:
    explicit GpioPin(int pin, Mode mode = OUTPUT_PWM);
    static int pwm_channel(int pin);
    void analogWrite(uint16_t cc, uint16_t top) const;
    [[nodiscard]] bool isPressed();
};

#endif //FADERINATOR_RGBLED_H
