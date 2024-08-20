//
// Created by Noah on 16.08.2024.
//

#include "Application.h"

#include <adc_rp2040.h>
#include <uart_rp2040.h>
#include <task.h>

#include "io/DataPacket.h"
#include "io/SerialConnection.h"

Application::Application(uart_rp2040 &uart) : serial(uart),
                                              pageUp(S1), pageDown(S2) {
    serial.leds[2].set_on_color(0xEE0000);

    for (int i = 0; i < PAGE_SIZE; ++i) {
        wires[i].deactivate();
    }

    for (int i = 0; i < PAGE_SIZE; ++i) {
        wires[i].reactivate();
        while (!displays[i].begin(2, 0x3c)) {
            serial.print(("SSD1306 allocation failed for %d\n"), i);
        }
        displays[i].clearDisplay();
        displays[i].print("Starting");
        displays[i].drawChar(118, 54, '0' + i, 1, 0, 1);
        displays[i].display();
        wires[i].deactivate();
    }

    serial.reset();
    serial << DataPacket{{DataPacket::PAGE_CHANGE, page, page}};
}

void Application::loop() {
    handleButtons();

    for (uint8_t i = 0; i < PAGE_SIZE; i++) {
        int val = faders[i].update();
        if (val != -1) {
            serial << DataPacket{{DataPacket::POSITION, static_cast<uint8_t>(i + page * PAGE_SIZE), (uint16_t) val}};
        }
    }

    handleConfigurationPackets();
}

void Application::handleConfigurationPackets() {
    ConfigurationPacket c = serial.pollPacket();
    while (c.type != ConfigurationPacket::IGNORE) {
        serial.leds[2] = 1;
        uint8_t localId = c.identifier - page * PAGE_SIZE;
        serial.print("id: %d,", localId);
        serial.print("type: %d,", c.type);
        switch (c.type) {
            case ConfigurationPacket::DAMAGED:
                if (reRequestTimeout < task::millis()) {
                    reRequestTimeout = task::millis() + 500;
                    updatePage(true); // resends everything of importance
                }
                break;
            case ConfigurationPacket::Position:
                faders[localId].setTarget(c.value);
                break;
            case ConfigurationPacket::RESET_CON:
                serial.reset();
                serial.print("%x", c.type);
                break;
            case ConfigurationPacket::Image:
                serial.print("starting with %d\n", c.identifier);
                wires[localId].reactivate();
                displays[localId].clearDisplay();
                displays[localId].drawCenteredBitmap<64>(serial.lastBufferedImage);
                displays[localId].drawChar(118, 54, '0' + c.identifier, 1, 0, 1);
                displays[localId].display();
                wires[localId].deactivate();
                serial.print("fin\n");
                break;
            case ConfigurationPacket::Display_Text:
                serial.print("starting text with %d\n", c.identifier);
                wires[localId].reactivate();
                displays[localId].clearDisplay();
                displays[localId].drawChar(118, 54, '0' + c.identifier, 1, 0, 1);
                displays[localId].print(serial.lastReadString);
                serial.print(serial.lastReadString);
                displays[localId].display();
                wires[localId].deactivate();
                serial.print(" - fin\n");
                break;
            case ConfigurationPacket::Mode:
            case ConfigurationPacket::IGNORE:
                break;
        }
        c = serial.pollPacket();
    }
    serial.leds[2] = 0;
}

void Application::handleButtons() {
    if (page < 3 && pageUp.isDown()) {
        page += 1;
        serial.print("Up to %d\n", page);
        updatePage();
    } else if (page != 0 && pageDown.isDown()) {
        page -= 1;
        serial.print("Down to %d\n", page);
        updatePage();
    }
}

void Application::updatePage(bool partial) {
    serial << DataPacket{{DataPacket::PAGE_CHANGE, page, page}};
    task::sleep_ms(10);
    if (partial) return;

    for (int i = 0; i < PAGE_SIZE; ++i) {
        wires[i].reactivate();
        displays[i].clearDisplay();
        displays[i].drawChar(118, 54, '0' + i + page * PAGE_SIZE, 1, 0, 1);
        displays[i].display();
        wires[i].deactivate();
    }
}
