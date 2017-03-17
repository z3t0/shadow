

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


// Bit twiddling
// Set a bit
// BYTE |= (1 << i)

// Clear a bit
// BYTE &= ~(1 << i);

// Toggle a bit
// BYTE ^= (1 << i)

// Forward declaration
void init();
void loop();

// Config

// DEBUG_LED
#define DEBUG_LED_PIN PB5 // D13
#define DEBUG_LED_DDRB DDRB
#define DEBUG_LED_PORT PORTB

// Debugging
void debug_led(int state) {
    if (state == 1) {
        // ON
        DEBUG_LED_PORT |= _BV(DEBUG_LED_PIN);
    }
    else if (state == 0) {
        // OFF
        DEBUG_LED_PORT &= ~_BV(DEBUG_LED_PIN);
    }
}

void debug_led_prepare() {
    DEBUG_LED_DDRB |= (1 << PB5);   // OUTPUT
    debug_led(0);
}

#endif