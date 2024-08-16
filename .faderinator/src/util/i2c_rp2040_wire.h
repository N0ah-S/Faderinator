//
// Created by Noah on 14.08.2024.
//

#ifndef FADERINATOR_I2C_RP2040_WIRE_H
#define FADERINATOR_I2C_RP2040_WIRE_H

#include <cstdio>
#include <i2c_rp2040.h>
#include <task.h>
#include "util.h"

class i2c_rp2040_wire : public i2c_rp2040 {

    byte buffer[128]{};
    byte address = 0;
    int position = 0;

public:
    i2c_rp2040_wire(uint8_t     index,
                    gpio_pin_t  sda_pin,
                    gpio_pin_t  scl_pin,
                    uint16_t    mode) : i2c_rp2040(index, sda_pin, scl_pin, mode) {

    }

    void beginTransmission(signed char i) {
        address = i;
    }

    uint16_t endTransmission() {
        //printf("Send %d bytes\n", position);
        uint16_t ret = this->i2cWrite(address, buffer, position);
        position = 0;
        return ret;
    }

    void write(uint8_t data) {
        buffer[position++] = data;
    }

    void begin() {
        address = 0; // broadcast???
    }

    int16_t i2cRead(uint16_t addr, uint8_t *rxbuf, uint16_t len, int timeout, bool sendStop = true) {
        unsigned long until = task::millis() + timeout;
        if (!_initialized) initialize();

        _i2c->IC_ENABLE.ENABLE = 0;
        _i2c->IC_TAR = addr;
        _i2c->IC_ENABLE.ENABLE = 1;

        int byte_ctr;
        for (byte_ctr = 0; byte_ctr < len; ++byte_ctr) {
            bool first = byte_ctr == 0;
            bool last  = byte_ctr == len - 1;

            while(_i2c->IC_TXFLR == 16) {
                if(until < task::millis()) return -1;
            };

            uint32_t cmd = 1 << 8;
            if (first && _restart_on_next) {
                cmd |= (1 << 10);
            }
            if (last && sendStop) {
                cmd |= (1 << 9);
            }
            _i2c->IC_DATA_CMD = cmd;

            while(_i2c->IC_RXFLR == 0) {
                if(until < task::millis()) return -1;
            };

            *rxbuf++ = _i2c->IC_DATA_CMD.DAT;
        }
        _restart_on_next = !sendStop;
        return len;
    }

    void reactivate() {
        _sda.setSEL(GPIO_CTRL_FUNCSEL__i2c);
    }


    void deactivate() {
        _initialized = false;
        _sda.setSEL(GPIO_CTRL_FUNCSEL__null);
    }

    void switchSDA(gpio_pin_t new_pin) {
        _sda.setSEL(GPIO_CTRL_FUNCSEL__null);
        _sda.setGpio(new_pin);
        _sda.setSEL(GPIO_CTRL_FUNCSEL__i2c);
    }

    friend i2c_rp2040;
};

#endif //FADERINATOR_I2C_RP2040_WIRE_H
