//
// Created by Noah on 10.08.2024.
//

#ifndef FADERINATOR_DATAPACKET_H
#define FADERINATOR_DATAPACKET_H

#include "../util/util.h"

union DataPacket {

    enum Type : uint8_t {
        POSITION = 1, STATE, PAGE_CHANGE, DEBUG_PRINT = 100
    };

    struct {
        // layout
        Type type{};
        /** virtual fader - fader+page */
        uint8_t identifier{};
        /** either position or state */
        uint16_t value{};
    };
    uint8_t data[4];
};


#endif //FADERINATOR_DATAPACKET_H
