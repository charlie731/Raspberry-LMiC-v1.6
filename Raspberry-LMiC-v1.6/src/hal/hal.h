/*******************************************************************************
 * Charlie 2017 Altec 
 *
 * This the HAL to run LMIC on top of the Raspberry environment.
 *******************************************************************************/
#ifndef _hal_hal_h_
#define _hal_hal_h_

static const int NUM_DIO = 3;

struct lmic_pinmap {
    u1_t nss;
    u1_t rxtx;
    u1_t rst;
    u1_t dio[NUM_DIO];
};

// Use this for any unused pins.
const u1_t LMIC_UNUSED_PIN = 0xff;

// Declared here, to be defined an initialized by the application
extern const lmic_pinmap lmic_pins;

#endif // _hal_hal_h_
