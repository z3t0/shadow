// sumobot.h
// Copyright (c) 2017 Rafi Khan

#ifndef SUMOBOT_H
#define SUMOBOT_H

/******************************************************************************
**** Configuration
******************************************************************************/
/// Modifiers
#define DEBUG 1
#define RELEASE 0

/// Constants
#define F_CPU 16000000UL

/// Includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Libraries
#if DEBUG
#include "USART.h"
#endif

/******************************************************************************
**** Program Flow
******************************************************************************/
// Forward Declaration
void init();    // Run once at the beginning
void loop();    // Run forever afterwards

/******************************************************************************
**** Motor Control
******************************************************************************/
// TODO: Implement motor control
// Set Motors
void setLeftMotor(int pwm);
void setRightMotor(int pwm);
/******************************************************************************
**** Line Sensors : QRD1114 with internal pull up
******************************************************************************/
/// PINOUT
// TODO: Find an interrupt and change ddr, pin, port etc.
// Back : A0
// #define LINE_BACK_BIT PC0
// #define LINE_BACK_PORT PORTC
// #define LINE_BACK_DDR DDRC
// #define LINE_BACK_PIN PINC

// Left : A1
#define LINE_LEFT_BIT PD2
#define LINE_LEFT_PORT PORTD
#define LINE_LEFT_DDR DDRD
#define LINE_LEFT_PIN PIND

// Right : A2
#define LINE_RIGHT_BIT PD3
#define LINE_RIGHT_PORT DDRD
#define LINE_RIGHT_DDR PORTD
#define LINE_RIGHT_PIN PIND

/// Prepares Line sensor pins as input and sets up interrupts
void prepare_line() {
    // Set input
    // LINE_BACK_DDR &= ~_BV(LINE_BACK_BIT);
    LINE_LEFT_DDR &= ~_BV(LINE_LEFT_BIT);
    LINE_RIGHT_DDR &= ~_BV(LINE_RIGHT_BIT);

    // Enable pullup
    // LINE_BACK_PORT |= _BV(LINE_BACK_BIT);
    LINE_LEFT_PORT |= _BV(LINE_LEFT_BIT);
    LINE_RIGHT_PORT |= _BV(LINE_RIGHT_BIT);

    // Interrupts
    //      on LOW for INT0 or INT1
    EICRA &= ~(_BV(ISC01) | _BV(ISC00) | _BV(ISC10) | _BV(ISC11));
    EIMSK |= (_BV(INT0) | _BV(INT1));     // Turns on INT0 and INT1
}

/// Interrupts called when White is detected
ISR (INT0_vect) {
    printString("White!");
}

ISR (INT1_vect) {
    // printString("White!");
}

/******************************************************************************
**** Sonar Sensor : HC-SR04
******************************************************************************/
/// PINOUT
#define PING_TRIG_BIT PD7
#define PING_TRIG_PORT DDRD
#define PING_ECHO PD6
#define PING_DDR DDRB
#define PING_PORT PORTB
#define PING_ECHO_PIN PINB

#define PING_MAX_CM 100

volatile uint16_t ref;
volatile uint16_t overflow = 0;

void prepare_ping() {
    // Sensor
    DDRD |= _BV(PD7); //PD7 as output
    PORTD &= ~_BV(PD7); //PD7 to LOW
    DDRB &= ~_BV(PB0); //PB0 as input
    PORTB &= ~_BV(PB0); //PB0 no pullup

    // Timer
    TCNT1 = 0; //Clear TIMER1 value
    OCR1A = 159; // 10 us without prescaler
    TIMSK1 = _BV(OCIE1A); //Enable compare match interrupt
    TCCR1B = _BV(ICES1) | _BV(CS10); // Rising edge, no prescaler
}


ISR(TIMER1_COMPA_vect)
{
    if(PING_TRIG_PORT & _BV(PING_TRIG_BIT)) //Test if query pulse on
    {
        PING_TRIG_PORT &= ~_BV(PING_TRIG_BIT); //End query pulse
        TIMSK1 &= ~_BV(OCIE1A); // disable compare match interrupt
        TIMSK1 |= _BV(ICIE1) | _BV(TOIE1); // Enable input capture interrupt, over flow interrupt
    }
    else //Query pulse not started
    {
        PING_TRIG_PORT |= _BV(PING_TRIG_BIT); //Start query pulse
    }
    TCNT1=0; //Clear TIMER1 value
}

ISR(TIMER1_CAPT_vect)
{
    TCCR1B &= ~_BV(CS10); //Stop TIMER1

    if(PINB & _BV(PB0)) //Test if response pulse started
    {
        TCNT1 = 0;
        overflow = 0;
        TCCR1B &= ~_BV(ICES1); //Next timer trigger on falling edge
    }

    else //Response pulse ended
    {
        // TODO: use ref
        // TODO: optimize
        //        - overflow > 1 means > 100 cm
        ref = ((TCNT1 * (float) 0.0625) / 58 + (overflow * 70.62));
        TIMSK1 &= ~_BV(ICIE1); // disable input capture interrupt
        TIMSK1 |= _BV(OCIE1A);  // enable compare match interrupt

        TCCR1B |= _BV(ICES1); //Next timer trigger on rising edge
    }

    TCNT1 = 0; //Clear TIMER1 value
    TCCR1B |= _BV(CS10); //Start TIMER1
}

ISR(TIMER1_OVF_vect) {
    overflow++;
}

/******************************************************************************
**** Debugging
******************************************************************************/

/// RGB DEBUG LED
/// PINOUT
// Red
#define DEBUG_LED_R_BIT PC0 // D1
#define DEBUG_LED_R_DDR DDRC
#define DEBUG_LED_R_PORT PORTC

// Green
#define DEBUG_LED_G_BIT PC1 // D2
#define DEBUG_LED_G_DDR DDRC
#define DEBUG_LED_G_PORT PORTC

// Blue
#define DEBUG_LED_B_BIT PC2 // D3
#define DEBUG_LED_B_DDR DDRC
#define DEBUG_LED_B_PORT PORTC


/// Sets the color
// r -> 0 or 1 : enable/disable red led
// g -> 0 or 1 : enable/disable green led
// b -> 0 or 1 : enable/disable blue led
void debug_led(int r, int g, int b) {
    if (!r) { // ON : RED
        DEBUG_LED_R_PORT |= _BV(DEBUG_LED_R_BIT);
    }

    else { // OFF: RED
        DEBUG_LED_R_PORT &= ~_BV(DEBUG_LED_R_BIT);
    }

    if (!g) { // ON : GREEN
        DEBUG_LED_G_PORT |= _BV(DEBUG_LED_G_BIT);
    }

    else { // OFF: GREEN
        DEBUG_LED_G_PORT &= ~_BV(DEBUG_LED_G_BIT);
    }

    if (!b) { // ON : BLUE
        DEBUG_LED_B_PORT |= _BV(DEBUG_LED_B_BIT);
    }

    else { // OFF: BLUE
        DEBUG_LED_B_PORT &= ~_BV(DEBUG_LED_B_BIT);
    }
}

/// Prepares ports
// Sets DDR to output and defaults to RED
void prepare_debug_led() {
    // Set Output
    DEBUG_LED_R_DDR |= _BV(DEBUG_LED_R_BIT);
    DEBUG_LED_G_DDR |= _BV(DEBUG_LED_G_BIT);
    DEBUG_LED_B_DDR |= _BV(DEBUG_LED_B_BIT);

    // Start with Red LED
    debug_led(1, 0, 0);
}
#endif