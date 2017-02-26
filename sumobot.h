

#ifndef SUMOBOT_H
#define SUMOBOT_H

// Modifiers
#define DEBUG 1
#define RELEASE 0

// Constants
#define F_CPU 16000000UL

// Includes
#include <avr/io.h>
#include <util/delay.h>

// Forward declaration
void init();
void loop();

// Config
#define DEBUG_LED_PIN PB5 // D13
#define DEBUG_LED_PORT DDRB

// Debugging
void debug_led(int state) {
    if (state) {
        // ON
        DEBUG_LED_PORT |= _BV(DEBUG_LED_PIN);
    }
    else {
        // OFF
        DEBUG_LED_PORT &= ~ _BV(DEBUG_LED_PIN);
    }
}

void debug_led_prepare() {
    DEBUG_LED_PORT |= (1 << PB5);
    debug_led(0);
}

#endif