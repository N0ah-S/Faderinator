//
// Created by Noah on 13.08.2024.
//

#include <yahal_String.h>
#include <cstdio>
#include <cstdarg>
#include <task.h>
#include "SerialConnection.h"

SerialConnection::SerialConnection(uart_rp2040 &uart) : leds(WS2812_PIN, LED_COUNT), uart(uart) {
    leds[0] = 0x000088;

    uart.uartAttachIrq([&](char input) {
        if (writePos > readPos && writePos % BUFFER_SIZE == readPos % BUFFER_SIZE) {
            printInt("Buffer Overflow s: %d ...\n", writePos);
            return;
        }
        buffer[writePos++ % BUFFER_SIZE] = input;
    });

    inst = this;
}

int SerialConnection::availableBytes() const {
    return writePos - readPos;
}

void SerialConnection::send(DataPacket data) {
    uint8_t check = 0;
    for (uint8_t byte: data.data) {
        //printf("Send byte %d\n", byte);
        check ^= byte;
        uart.putc(byte);
    }
    uart.putc(check);
}

byte SerialConnection::read() {
    unsigned long until = task::millis() + 200;
    while (availableBytes() == 0) {
        if(until < task::millis()) return 0;
    }
    return buffer[readPos++ % BUFFER_SIZE];
}

ConfigurationPacket SerialConnection::pollPacket() {
    if (availableBytes() < 5) return ConfigurationPacket{.type = ConfigurationPacket::CommandType::IGNORE};
    ConfigurationPacket received{};
    std::copy(buffer + readPos, buffer + readPos + 4, std::begin(received.data));
    if(buffer[readPos] == ConfigurationPacket::Image) {
        readPos += 2; // shared packet "header"
        handleImage();
        return received;
    } else if(buffer[readPos] == ConfigurationPacket::Display_Text) {
        readPos += 2; // shared packet "header"
        handleText();
        return received;
    }
    readPos += 4;

    byte check = 0;
    for (byte b: received.data) {
        print("%d- ", b);
        check ^= b;
    }
    byte compareTo = read();
    if (check == compareTo) {
        print("ok\n");
        return received;
    } else {
        print(" - check failed with %d\n", compareTo ^ check);
        print("with %d\n", compareTo);
        print("with %d\n", check);
        return ConfigurationPacket{.type = ConfigurationPacket::CommandType::DAMAGED};
    }
}

SerialConnection &operator<<(SerialConnection &stream, const DataPacket packet) {
    stream.send(packet);
    return stream;
}

SerialConnection &operator<<(SerialConnection &stream, const YAHAL::String &string) {
    int i = 0;
    while (i < string.size()) {
        DataPacket p{.type = DataPacket::DEBUG_PRINT};
        int j = 1;
        while (i < string.size() && j < 4) {
            p.data[j++] = string[i++];
        }
        stream.send(p);
    }
    return stream;
}

void SerialConnection::printInt(const char *text, int i) {
    char outbuf[128];

    snprintf(outbuf, sizeof(outbuf), text, i);
    *this << YAHAL::String(outbuf);
}

void SerialConnection::print(const char *text, ...) {
    char outbuf[128];
    va_list args;
    va_start(args, text);

    snprintf(outbuf, sizeof(outbuf), text, args);
    va_end(args);
    *this << YAHAL::String(outbuf);
}

void SerialConnection::print(const char *text, byte b) {
    char outbuf[128];

    snprintf(outbuf, sizeof(outbuf), text, b);
    *this << YAHAL::String(outbuf);
}

void SerialConnection::reset() {
    leds[5].set_color(0xFF0000);
    for (int i = 0; i < 100; ++i) {
        uart.putc(0);
    }
    uart.putc(1);
    leds[5].set_color(0x0000);
}

void SerialConnection::handleImage() {
    print("Got an image\n");
    while (availableBytes() < 64*8) {}
    if(lastBufferedImage != nullptr) delete lastBufferedImage;
    lastBufferedImage = new ByteRegion<64 * 8, BUFFER_SIZE>(buffer, readPos);
    readPos += 64*8; // payload
    byte check = read();
    if(check == lastBufferedImage->checksumXOR()) {
        print("nice\n");
    }
}

void SerialConnection::handleText() {
    byte length = read();
    lastReadString = new char [length + 1];
    for (int i = 0; i < length; ++i) {
        lastReadString[i] = read();
    }
    lastReadString[length] = 0;
    read();
}


