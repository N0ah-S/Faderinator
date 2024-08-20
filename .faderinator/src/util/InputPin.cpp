//
// Created by Noah on 16.08.2024.
//

#include "InputPin.h"

InputPin::InputPin(gpio_pin_t pin) : gpio_rp2040_pin(pin) {
    gpioMode(GPIO::INPUT | GPIO::PULLUP);
}

bool InputPin::isDown() {
    if (!gpioRead()) {
        counter++;
    } else if (counter > 0) {
        counter--;
    }

    if (counter > SENSITIVITY) {
        counter = -20; // anti spamming
        return true;
    }
    return false;
}
