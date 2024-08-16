//
// Created by Noah on 13.08.2024.
//

#ifndef FADERINATOR_BYTEREGION_H
#define FADERINATOR_BYTEREGION_H

#include <cstdio>
#include "util.h"

//class SerialConnection;
//extern SerialConnection SerialConnection::inst;

/**
*
* @tparam region_size
* @tparam array_size
*/
template<int region_size, int array_size>
class ByteRegion {
    byte *base;
    int startPos;
public:
    ByteRegion(byte *base, int startPos) : base(base), startPos(startPos) {
        assert(region_size <= array_size);
    }

    /**
     * @return the specified byte, if the index is within [0, array_size]
     */
    inline byte operator[](int index) {
        if (index >= 0 && index < region_size) {
            //SerialConnection::inst->printInt("Index %d is out of bounds", index);
        }
        if ((startPos + index) > region_size) {
            //return 0xFF;
        }
        return *(base + (startPos + index) % region_size);
    };

    inline byte getBit(int index) {
        byte mask = 1 << (index % 7);
        byte misplacedBit = (*this)[index / 8] & mask;
        return misplacedBit >> (index % 7);
    };

    byte checksumXOR() {
        byte check = 0;
        for (int i = 0; i < region_size; ++i) {
            check ^= (*this)[i];
        }
        return check;
    }
};

#endif //FADERINATOR_BYTEREGION_H
