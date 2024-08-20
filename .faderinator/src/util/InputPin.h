//
// Created by Noah on 16.08.2024.
//

#ifndef FADERINATOR_INPUTPIN_H
#define FADERINATOR_INPUTPIN_H


#include <gpio_rp2040.h>

/**
 * small wrapper for averaged/filtered button inputs
 */
class InputPin : public gpio_rp2040_pin {
public:
    explicit InputPin(gpio_pin_t pin);

    bool isDown();

private:
    int counter = 0;
    static constexpr int SENSITIVITY = 30;
};


#endif //FADERINATOR_INPUTPIN_H
