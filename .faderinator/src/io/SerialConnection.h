//
// Created by Noah on 13.08.2024.
//

#ifndef FADERINATOR_SERIALCONNECTION_H
#define FADERINATOR_SERIALCONNECTION_H


#include <uart_rp2040.h>
#include "DataPacket.h"
#include "ConfigurationPacket.h"


#include "../util/ByteRegion.h"


class SerialConnection {

    static constexpr int BUFFER_SIZE = 128 * 64 * 4 + 512; // 4 Images plus storing room ~34 kB

public:
    // poor mans singelton
    inline static SerialConnection* inst = nullptr;
    // I think a getter would be overkill for this one
    ByteRegion<64*8,BUFFER_SIZE>* lastBufferedImage = nullptr;
    char* lastReadString;

    explicit SerialConnection(uart_rp2040 &uart);
    [[nodiscard]] int availableBytes() const;
    void send(DataPacket data);
    ConfigurationPacket pollPacket();
    byte read();

    void print(const char *text, ...);

    void reset();

    void print(const char *text, byte b);
    void printInt(const char *text, int i);


    LedArray leds;
private:
    // debug
    uart_rp2040 &uart;
    byte buffer[BUFFER_SIZE]{};
    int readPos = 0;
    int writePos = 0;


    void handleImage();

    void handleText();
};

SerialConnection &operator<<(SerialConnection &stream, DataPacket packet);


#endif //FADERINATOR_SERIALCONNECTION_H
