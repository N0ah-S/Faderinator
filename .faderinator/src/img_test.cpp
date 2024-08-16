//
// Created by Noah on 15.08.2024.
//
#ifndef dddd
#define dddd

#include <uart_rp2040.h>
#include <posix_io.h>
#include <cstdio>
#include "util/i2c_rp2040_wire.h"
#include "util/util.h"
#include "Adafruit_SSD1306.h"

#define delay(x) task::sleep_ms(x)

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char logo_bmp[] =
        { 0b00000000, 0b11000000,
          0b00000001, 0b11000000,
          0b00000001, 0b11000000,
          0b00000011, 0b11100000,
          0b11110011, 0b11100000,
          0b11111110, 0b11111000,
          0b01111110, 0b11111111,
          0b00110011, 0b10011111,
          0b00011111, 0b11111100,
          0b00001101, 0b01110000,
          0b00011011, 0b10100000,
          0b00111111, 0b11100000,
          0b00111111, 0b11110000,
          0b01111100, 0b11110000,
          0b01110000, 0b01110000,
          0b00000000, 0b00110000 };
static int lol() {
    uart_rp2040 uart(0, 0,1,115200);
    posix_io::inst.register_stdout(uart);

    printf("init\n");
    i2c_rp2040_wire i2c1(1, 2, 3, 0);
    Adafruit_SSD1306 display(128, 64, &i2c1);
    while(!display.begin(2, 0x3c )) {
        printf(("SSD1306 allocation failed\n"));
    }
    printf(("D1 didn't fail\n"));

    display.clearDisplay();
    for(int16_t i=0; i<std::max(display.width(),display.height())/2; i+=2) {
        display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
        display.display();
        delay(1);
    }
    i2c1.deactivate();
    i2c_rp2040_wire i2c2(1, 6, 3, 0);
    Adafruit_SSD1306 display2(128, 64, &i2c2);
    while(!display2.begin(2, 0x3c )) {
        printf(("SSD1306 allocation failed\n"));
    }
    printf(("D2 didn't fail\n"));
    for(int16_t i=0; i<std::max(display.width(),display.height())/2; i+=2) {
        display2.clearDisplay();
        display2.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
        display2.display();
        delay(1);
    }

    display2.clearDisplay();

    delay(2000);
    display2.drawBitmap(
            (display.width()  - LOGO_WIDTH ) / 2,
            (display.height() - LOGO_HEIGHT) / 2,
            logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display2.display();
    delay(1000);
    display.clearDisplay();

    return 0;
}

#endif