// ---------------------------------------------
//           This file is part of
//      _  _   __    _   _    __    __
//     ( \/ ) /__\  ( )_( )  /__\  (  )
//      \  / /(__)\  ) _ (  /(__)\  )(__
//      (__)(__)(__)(_) (_)(__)(__)(____)
//
//     Yet Another HW Abstraction Library
//      Copyright (C) Andreas Terstegge
//      BSD Licensed (see file LICENSE)
//
// ---------------------------------------------
//

#include "i2c_rp2040_copy.h"
#include "system_rp2040.h"
#include <cassert>

using namespace _IO_BANK0_;
using namespace _RESETS_;

i2c_rp2040_copy::i2c_rp2040_copy(uint8_t      index,
                                 gpio_pin_t   sda_pin,
                                 gpio_pin_t   scl_pin,
                                 uint16_t     mode)
    : _initialized(false), _index(index), _sda(sda_pin), _scl(scl_pin), _mode(mode),
       _restart_on_next(false) {

    assert(index < 2);
    assert((sda_pin <= 28) && (scl_pin <= 29));
    if (index == 0) {
        assert( (((sda_pin-0) % 4) == 0) &&
                (((scl_pin-1) % 4) == 0) );
    } else {
        assert( (((sda_pin-2) % 4) == 0) &&
                (((scl_pin-3) % 4) == 0) );
    }
    _i2c     = (index==0) ? &I2C0     : &I2C1;
    _i2c_set = (index==0) ? &I2C0_SET : &I2C1_SET;
    _i2c_clr = (index==0) ? &I2C0_CLR : &I2C1_CLR;
}

i2c_rp2040_copy::~i2c_rp2040_copy() {
    // Wait for pending operations
//    while (_EUSCI_STATW & EUSCI_A_STATW_BUSY);

    // Reset CTLW0 register to default values
    // (EUSCI_A is in reset state)
//    _EUSCI_CTLW0 = EUSCI_A_CTLW0_SWRST;
}

void i2c_rp2040_copy::initialize() {
    // Take SPI out of reset state
    if (_index)  RESETS_CLR.RESET.i2c1 = 1;
    else         RESETS_CLR.RESET.i2c0 = 1;
    // Configure GPIO pins SDA and SCL
    _sda.setSEL(GPIO_CTRL_FUNCSEL__i2c);
    _scl.setSEL(GPIO_CTRL_FUNCSEL__i2c);

    // Disable unit
    _i2c->IC_ENABLE.ENABLE = 0;

    // Configure IC_CON register
    _i2c->IC_CON.MASTER_MODE = 1;
    _i2c->IC_CON.SPEED = IC_CON_SPEED__FAST;
    _i2c->IC_CON.IC_10BITADDR_MASTER = 0;
    _i2c->IC_CON.IC_RESTART_EN = 1;
    _i2c->IC_CON.IC_SLAVE_DISABLE = 1;
    _i2c->IC_CON.TX_EMPTY_CTRL = 1;

    // FIFO watermark levels
    _i2c->IC_TX_TL = 0;
    _i2c->IC_RX_TL = 0;

    // Finally set speed and enable unit
//    setSpeed(400000);
    _initialized = true;
}

int16_t i2c_rp2040_copy::i2cRead (uint16_t addr, uint8_t *rxbuf,
                                  uint16_t len, bool sendStop) {
    if (!_initialized) initialize();

    _i2c->IC_ENABLE.ENABLE = 0;
    _i2c->IC_TAR = addr;
    _i2c->IC_ENABLE.ENABLE = 1;

    int byte_ctr;
    for (byte_ctr = 0; byte_ctr < len; ++byte_ctr) {
        bool first = byte_ctr == 0;
        bool last  = byte_ctr == len - 1;

        while(_i2c->IC_TXFLR == 16) ;

        uint32_t cmd = 1 << 8;
        if (first && _restart_on_next) {
            cmd |= (1 << 10);
        }
        if (last && sendStop) {
            cmd |= (1 << 9);
        }
        _i2c->IC_DATA_CMD = cmd;

        while(_i2c->IC_RXFLR == 0) ;

        *rxbuf++ = _i2c->IC_DATA_CMD.DAT;
    }
    _restart_on_next = !sendStop;
    return len;
}

int16_t i2c_rp2040_copy::i2cWrite(uint16_t addr, uint8_t *txbuf,
                                  uint16_t len, bool sendStop) {
    if (!_initialized) initialize();

    _i2c->IC_ENABLE.ENABLE = 0;
    _i2c->IC_TAR = addr;
    _i2c->IC_ENABLE.ENABLE = 1;

    int byte_ctr;
    for (byte_ctr = 0; byte_ctr < len; ++byte_ctr) {
        bool first = byte_ctr == 0;
        bool last  = byte_ctr == len - 1;

        uint32_t cmd = *txbuf++;
        if (first && _restart_on_next) {
            cmd |= (1 << 10);
        }
        if (last && sendStop) {
            cmd |= (1 << 9);
        }
        _i2c->IC_DATA_CMD = cmd;

        // Wait until the transmission of the address/data from the internal
        // shift register has completed. For this to function correctly, the
        // TX_EMPTY_CTRL flag in IC_CON must be set. The TX_EMPTY_CTRL flag
        // was set in i2c_init.
        while(!_i2c->IC_RAW_INTR_STAT.TX_EMPTY) ;
    }
    _restart_on_next = !sendStop;
    return len;
}

void i2c_rp2040_copy::setSpeed(uint32_t freq) {
    if (!_initialized) initialize();

    // I2C is synchronous design that runs from clk_sys
    uint32_t freq_in = CLK_SYS;

    // TODO there are some subtleties to I2C timing which we are completely ignoring here
    uint32_t period = (freq_in + freq / 2) / freq;
    uint32_t lcnt = period * 3 / 5; // oof this one hurts
    uint32_t hcnt = period - lcnt;

    // Per I2C-bus specification a device in standard or fast mode must
    // internally provide a hold time of at least 300ns for the SDA signal to
    // bridge the undefined region of the falling edge of SCL. A smaller hold
    // time of 120ns is used for fast mode plus.
    uint32_t sda_tx_hold_count;
    if (freq < 1000000) {
        // sda_tx_hold_count = freq_in [cycles/s] * 300ns * (1s / 1e9ns)
        // Reduce 300/1e9 to 3/1e7 to avoid numbers that don't fit in uint.
        // Add 1 to avoid division truncation.
        sda_tx_hold_count = ((freq_in * 3) / 10000000) + 1;
    } else {
        // sda_tx_hold_count = freq_in [cycles/s] * 120ns * (1s / 1e9ns)
        // Reduce 120/1e9 to 3/25e6 to avoid numbers that don't fit in uint.
        // Add 1 to avoid division truncation.
        sda_tx_hold_count = ((freq_in * 3) / 25000000) + 1;
    }
    assert(sda_tx_hold_count <= lcnt - 2);

    _i2c->IC_ENABLE.ENABLE = 0;
    // Always use "fast" mode (<= 400 kHz, works fine for standard mode too)
    _i2c->IC_CON.SPEED = IC_CON_SPEED__FAST;
    _i2c->IC_FS_SCL_HCNT = hcnt;
    _i2c->IC_FS_SCL_LCNT = lcnt;
    _i2c->IC_FS_SPKLEN   = lcnt < 16 ? 1 : lcnt / 16;
    _i2c->IC_SDA_HOLD.IC_SDA_TX_HOLD = sda_tx_hold_count;

    _i2c->IC_ENABLE.ENABLE = 1;
    return; // freq_in / period;
}
