//
// Created by Noah on 13.08.2024.
//

#ifndef FADERINATOR_CONFIGURATIONPACKET_H
#define FADERINATOR_CONFIGURATIONPACKET_H

#include "../util/util.h"

union ConfigurationPacket {
    enum CommandType : byte {
        RESET_CON = 0, Position, Image, Display_Text, Mode, IGNORE = 101, DAMAGED = 102
    };
    struct {
        CommandType type;
        byte identifier;
        uint16_t value;
    };
    byte data[4];

};

#endif //FADERINATOR_CONFIGURATIONPACKET_H
