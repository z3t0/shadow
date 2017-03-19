

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
#include "USART.h"


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

// Set Motors
void setLeftMotor(int pwm);
void setRightMotor(int pwm);

void readIRSensor(int pint);

// Config

// DEBUG_LED
#define DEBUG_LED_R_INT PD1 // D0
#define DEBUG_LED_R_DDR DDRD
#define DEBUG_LED_R_PORT PORTD

#define DEBUG_LED_G_INT PD2
#define DEBUG_LED_G_DDR DDRD
#define DEBUG_LED_G_PORT PORTD

#define DEBUG_LED_B_INT PD3
#define DEBUG_LED_B_DDR DDRD
#define DEBUG_LED_B_PORT PORTD

// IR Sensors
#define IR_LEFT_INT PD4
#define IR_LEFT_PIN PIND
#define IR_LEFT_DDR DDRD
#define IR_LEFT_PORT PORTD

#define IR_RIGHT_INT PD5
#define IR_RIGHT_PIN PIND
#define IR_RIGHT_DDR DDRD
#define IR_RIGHT_PORT PORTD

// Ping Sensor
#define PING_INT PB0
#define PING_ECHO PD6
#define PING_PIN PINB
#define PING_DDR DDRB
#define PING_PORT PORTB

// Sensors

// Object Sensors

void prepare_IR_sensors() {
    IR_LEFT_DDR &= ~_BV(IR_LEFT_INT);
    IR_RIGHT_DDR &= ~_BV(IR_RIGHT_INT);
}

struct ir_sensor {
    char left;
    char right;
};

// returns 1 for detected
struct ir_sensor read_IR_sensors() {
    struct ir_sensor data;

    data.left = !(IR_LEFT_PIN & _BV(IR_LEFT_INT));
    data.right = !(IR_RIGHT_PIN & _BV(IR_RIGHT_INT));

    return data;
}

// Ping Sensor


void read_ping() {
    // Pulse to high or pulse to low?

    // Pulse out : 5us
    PING_DDR |= _BV(PING_INT); // OUTPUT
    PING_PORT |= _BV(PING_INT);

    _delay_us(5);

    // Pulse in 
    // PING_PORT &= ~_BV(PING_INT);
    // Read pulse width

    // Convert
}

// Debugging
// Change debug led
void debug_led(int r, int g, int b) {
    if (!r) { // ON : RED
        DEBUG_LED_R_PORT |= _BV(DEBUG_LED_R_INT);
    }

    else { // OFF: RED
        DEBUG_LED_R_PORT &= ~_BV(DEBUG_LED_R_INT);
    }

    if (!g) { // ON : GREEN
        DEBUG_LED_G_PORT |= _BV(DEBUG_LED_G_INT);
    }

    else { // OFF: GREEN
        DEBUG_LED_G_PORT &= ~_BV(DEBUG_LED_G_INT);
    }

    if (!b) { // ON : BLUE
        DEBUG_LED_B_PORT |= _BV(DEBUG_LED_B_INT);
    }

    else { // OFF: BLUE
        DEBUG_LED_B_PORT &= ~_BV(DEBUG_LED_B_INT);
    }
}

void debug_led_toggle() {
    DEBUG_LED_G_PORT ^= _BV(DEBUG_LED_B_INT);
}

// Sets up debug LED
void prepare_debug_led() {
    // Set Output
    DEBUG_LED_R_DDR |= _BV(DEBUG_LED_R_INT);
    DEBUG_LED_G_DDR |= _BV(DEBUG_LED_G_INT);
    DEBUG_LED_B_DDR |= _BV(DEBUG_LED_B_INT);

    // Start with Red LED
    debug_led(1, 0, 0);
}

#endif