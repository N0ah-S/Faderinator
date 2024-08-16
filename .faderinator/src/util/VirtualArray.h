//
// Created by Noah on 05.05.2024.
//

#ifndef FADERINATOR_VIRTUALARRAY_H
#define FADERINATOR_VIRTUALARRAY_H

#include <cassert>
#include "../../../../../include/MCU/rp2040/RP2040.h"

using namespace _PADS_BANK0_;
using namespace _IO_BANK0_;
using namespace _SIO_;
using namespace _PWM_;

/**
 * Wrapper enabling direct array-like access to otherwise hard coded registers
 * @tparam type the type of object that should be returned. Technically only the length of this type is relevant.
 * @tparam offset offset between 'elements' in multiples
 * @tparam size maximum amount of elements that can be accessed; defaults to effectively infinity.
 * since there is usually only one possible offset and size per type this should be fine
 */
template<typename type, int offset = 1, int size = 0xfffffff>
class VirtualArray {
    type *base;
public:
    VirtualArray(type *base) : base(base) {} // NOLINT(*-explicit-constructor) [1]
    /**
     * @return the specified elements, if the index is within [0, size]
     */
    inline type *operator[](int index) {
        assert(index >= 0 && index < size);
        return base + index * offset;
    };
};

namespace VIO_BANK0 {
    VirtualArray<GPIO_CTRL_t, 2> GPIO_CTRL = &IO_BANK0.GPIO0_CTRL;
}

namespace VPWM {
    VirtualArray<CH_DIV_t, 5> CH_DIV = &PWM.CH0_DIV;
    VirtualArray<CH_TOP_t, 5> CH_TOP = &PWM.CH0_TOP;
    VirtualArray<CH_CC_t, 5> CH_CC = &PWM.CH0_CC;
}

// [1] Experts are still debating whether this is syntactic sugar or a crime against humanity ¯\_(ツ)_/¯

#endif //FADERINATOR_VIRTUALARRAY_H
