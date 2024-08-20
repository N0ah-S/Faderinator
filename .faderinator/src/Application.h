//
// Created by Noah on 16.08.2024.
//

#ifndef FADERINATOR_APPLICATION_H
#define FADERINATOR_APPLICATION_H


#include <uart_rp2040.h>
#include "io/SerialConnection.h"
#include "util/InputPin.h"
#include "Fader.h"
#include "util/i2c_rp2040_wire.h"
#include "Adafruit_SSD1306.h"

class Application {
public:
    explicit Application(uart_rp2040 &uart);

    void loop();

    static constexpr uart_mode_t MODE = UART::BITS_8 | UART::ODD_PARITY | UART::STOPBITS_2;
private:
    static constexpr uint8_t PAGE_SIZE = 3;

    SerialConnection serial;
    InputPin pageUp;
    InputPin pageDown;

    uint64_t reRequestTimeout = 0;
    uint8_t page = 0;

    Fader faders[PAGE_SIZE] = {
            Fader(24, 25, 1),
            Fader(22, 23, 2),
            Fader(14, 15, 3),
    };
    i2c_rp2040_wire wires[PAGE_SIZE] = {
            i2c_rp2040_wire(1, 2, 3, 0),
            i2c_rp2040_wire(1, 6, 3, 0),
            i2c_rp2040_wire(0, 4, 5, 0),
    };
    Adafruit_SSD1306 displays[PAGE_SIZE] = {
            Adafruit_SSD1306(128, 64, &wires[0]),
            Adafruit_SSD1306(128, 64, &wires[1]),
            Adafruit_SSD1306(128, 64, &wires[2]),
    };

    void handleButtons();

    void updatePage(bool partial = false);

    void handleConfigurationPackets();
};


#endif //FADERINATOR_APPLICATION_H
