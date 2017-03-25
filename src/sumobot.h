// sumobot.h
// Copyright (c) 2017 Rafi Khan

#ifndef SUMOBOT_H
#define SUMOBOT_H


// STATE MACHINE
enum STATE {
    DEFAULT,
    START,
    START_DELAY,
    FRONT_LINE,
    SPIN,
    LEFT_LINE,
    BACK_LINE,
    RIGHT_LINE,
    CHARGE_SLOW
};

volatile enum STATE state = START;
volatile enum STATE last_state = DEFAULT;
volatile enum STATE next_state = DEFAULT;
volatile enum STATE timer2_state = DEFAULT;

void set_state(enum STATE s) {
    last_state = state;
    state = s;
    stop_timer();
}

void set_next_state(enum STATE s)  {
    next_state = s;
}

void go_next_state() {
    set_state(next_state);
    // printString("next state");
}
/******************************************************************************
 **** Configuration
 ******************************************************************************/
/// Modifiers
#define DEBUG 1
#define RELEASE 1

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
// PINOUT: Left D5, Right D6
// Timer 0 : OC0B
#define MOTOR_LEFT_BIT PD5

// Timer 2 : OC2A
#define MOTOR_RIGHT_BIT PB3

// TODO: Test motor control
// Set Motors
void setLeftMotor(int pwm);
void setRightMotor(int pwm);

// 1000us : 62.5
// 1500us : 93.75 // 666 Hz
// 2000us: 125

#define MOTOR_FWD_FULL 125
#define MOTOR_FWD_HALF 109
#define MOTOR_FWD_QUARTER 101
#define MOTOR_STOP 93
#define MOTOR_REV_FULL 63
#define MOTOR_REV_QUARTER 86
#define MOTOR_REV_HALF 78

// Sets up motor outputs and timers
void prepare_motor() {
    // TODO: Clean up
    // Start at idle
    OCR0B = MOTOR_STOP;
    OCR0A = MOTOR_STOP;

    DDRD |= _BV(PD5);
    DDRD |= _BV(PD6);
    DDRB |= _BV(PB3);

    // Timer 0 : Left Motor
    // Top : OCR0B. PWM phase correct
    TCCR0B |= _BV(CS02);  // 256 prescaler

    // Phase correct mode, OCRA top
    TCCR0A |= _BV(WGM00) | _BV(WGM01);   // FAST PWM
    // TCCR0A |= _BV(WGM00);
    // TCCR0B |= _BV(WGM02);    
    // TCCR0A |= (_BV(COM0B1) | _BV(COM0B0)); // Inverted?
    TCCR0A |= _BV(COM0B1); // Clear on match
    TCCR0A |= _BV(COM0A1); // Clear on match
}

void left_motor(uint8_t top) {
    OCR0A = top;
}

void right_motor(uint8_t top) {
    OCR0B = top;
}

void stop_motor() {
    right_motor(MOTOR_STOP);
    left_motor(MOTOR_STOP);
}

void set_motor(uint8_t l, uint8_t r) {
    right_motor(r);
    left_motor(l);
}

/******************************************************************************
 **** Line Sensors : QRD1114 with internal pull up
 ******************************************************************************/
/// PINOUT
// Back : D9, PCINT1
#define LINE_BACK_BIT PB1
#define LINE_BACK_PORT PORTB
#define LINE_BACK_DDR DDRB
#define LINE_BACK_PIN PINB

// Left : D2, INT 0
#define LINE_LEFT_BIT PD2
#define LINE_LEFT_PORT PORTD
#define LINE_LEFT_DDR DDRD
#define LINE_LEFT_PIN PIND

// Right : D3, INT 1
#define LINE_RIGHT_BIT PD3
#define LINE_RIGHT_PORT PORTD
#define LINE_RIGHT_DDR DDRD
#define LINE_RIGHT_PIN PIND

/// Prepares Line sensor pins as input and sets up interrupts
void prepare_line() {
    // Set input
    LINE_BACK_DDR &= ~_BV(LINE_BACK_BIT);
    LINE_LEFT_DDR &= ~_BV(LINE_LEFT_BIT);
    LINE_RIGHT_DDR &= ~_BV(LINE_RIGHT_BIT);

    // Enable pullup
    LINE_BACK_PORT |= _BV(LINE_BACK_BIT);
    LINE_LEFT_PORT |= _BV(LINE_LEFT_BIT);
    LINE_RIGHT_PORT |= _BV(LINE_RIGHT_BIT);

    // Interrupts
    //      on LOW for INT0 or INT1
    // EICRA &= ~(_BV(ISC01) | _BV(ISC00) | _BV(ISC10) | _BV(ISC11));
    EICRA &= ~(_BV(ISC00) | _BV(ISC10)); // any logic change
    EIMSK |= (_BV(INT0) | _BV(INT1));     // Turns on INT0 and INT1

    // //      on change for PCINT0
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT1);  //set PCINT0 to trigger an interrupt on state change
}

// Interrupt called on White for Left
ISR (INT0_vect) {
    if(!(LINE_LEFT_PIN & _BV(LINE_LEFT_BIT)) && !(LINE_RIGHT_PIN & _BV(LINE_RIGHT_BIT))) {
        // FRONT
        set_state(FRONT_LINE);
        set_motor(MOTOR_REV_FULL, MOTOR_REV_FULL);
        set_timer(1000, SPIN); // forward for 500s

    }

    else if (!(LINE_LEFT_PIN & _BV(LINE_LEFT_BIT))) {
        // LEFT
        set_state(LEFT_LINE);
        set_motor(MOTOR_REV_FULL, MOTOR_REV_HALF);
        set_timer(1000, SPIN); // forward for 500s
    }
}

// Interrupt called on white for right
ISR (INT1_vect) {
    if(!(LINE_RIGHT_PIN & _BV(LINE_RIGHT_BIT))) {
        set_state(RIGHT_LINE);

        // set_motor(MOTOR_FWD_HALF, MOTOR_FWD_HALF);
        set_motor(MOTOR_REV_FULL, MOTOR_REV_HALF);
        set_timer(1000, SPIN); // forward for 500s   
    }
}

// Interrupt called for state change on PCINT0
ISR (PCINT0_vect) {
    // TODO: strategy for back
    if (!(LINE_BACK_PIN & _BV(LINE_BACK_BIT))) {
        // Back
        set_state(BACK_LINE);
        // printString("back line");          

        set_motor(MOTOR_FWD_FULL, MOTOR_FWD_FULL);
        set_timer(700, SPIN); // forward for 500s
    }
}

/******************************************************************************
 **** Sonar Sensor : HC-SR04
 ******************************************************************************/
/// PINOUT : Trig D7, Echo D8
#define PING_TRIG_BIT PD7
#define PING_TRIG_PORT PORTD
#define PING_TRIG_DDR DDRD

#define PING_ECHO_BIT PB0 // needed cause ICP1
#define PING_ECHO_DDR DDRB
#define PING_ECHO_PORT PORTB
#define PING_ECHO_PIN PINB

#define PING_MAX_CM 100

volatile uint16_t ref = 0;
volatile uint16_t overflow = 0;

void prepare_ping() {
    // Sensor
    PING_TRIG_DDR |= _BV(PING_TRIG_BIT); //PD7 as output
    PING_TRIG_PORT &= ~_BV(PING_TRIG_BIT); //PD7 to LOW
    PING_ECHO_DDR &= ~_BV(PING_ECHO_BIT); //PB0 as input
    PING_ECHO_PORT &= ~_BV(PING_ECHO_BIT); //PB0 no pullup

    // Timer
    TCNT1 = 0; //Clear TIMER1 value
    OCR1A = 159; // 10 us without prescaler
    TIMSK1 = _BV(OCIE1A); //Enable compare match interrupt
    TCCR1B = _BV(ICES1) | _BV(CS10); // Rising edge, no prescaler
}


ISR(TIMER1_COMPA_vect)
{
    if(PING_TRIG_DDR & _BV(PING_TRIG_BIT)) //Test if query pulse on
    {
        PING_TRIG_DDR &= ~_BV(PING_TRIG_BIT); //End query pulse
        TIMSK1 &= ~_BV(OCIE1A); // disable compare match interrupt
        TIMSK1 |= _BV(ICIE1) | _BV(TOIE1); // Enable input capture interrupt, over flow interrupt
    }
    else //Query pulse not started
    {
        PING_TRIG_DDR |= _BV(PING_TRIG_BIT); //Start query pulse
    }
    TCNT1=0; //Clear TIMER1 value
    // printString("comp");
}

ISR(TIMER1_CAPT_vect)
{
    TCCR1B &= ~_BV(CS10); //Stop TIMER1

    if(PING_ECHO_PIN & _BV(PING_ECHO_BIT)) //Test if response pulse started
    {
        TCNT1 = 0;
        overflow = 0;
        TCCR1B &= ~_BV(ICES1); //Next timer trigger on falling edge
    }

    else //Response pulse ended
    {
        //        - overflow > 1 means > 100 cm
        ref = ((TCNT1 * (float) 0.0625) / 58 + (overflow * 70.62));
        if (state != SPIN){
            TCNT1 = 0; //Clear TIMER1 value
            TCCR1B |= _BV(CS10); //Start TIMER1
            set_state(SPIN);        
        }

        char c[10]; 
        utoa(ref, c, 10);
        printString(c);

        if(ref < 70 && ref != 0) {
            // FOUND:
            // printString("found");
            set_state(CHARGE_SLOW);
        }

        else {
            set_state(SPIN);
        }

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
#define DEBUG_LED_G_BIT PC1/// D2
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

void toggle_red() {
    DEBUG_LED_R_PORT ^= _BV(DEBUG_LED_R_BIT);
}

volatile uint8_t timer2_counter;
volatile uint8_t timer2_target;
volatile uint8_t timer2_on = 0;

// Prepares Timer 2 for counting time for the sumobot moves
// this counts in 100 nano seconds = 0.01
void prepare_timer() {
    OCR2A = 157;

    TCCR2A |= (1 << WGM21);
    // Set to CTC Mode

    TIMSK2 |= (1 << OCIE2A);
    //Set interrupt on compare match
}

// delay in nanoseconds
void set_timer(int target, enum STATE next) {
    TCCR2B |= (_BV(CS20) | _BV(CS21) | _BV(CS22));
    // set prescaler to 1024 and starts PWM
    timer2_counter = 0;
    timer2_target = target / 10;
    timer2_state = state;


    timer2_on = 1;
    set_next_state(next);
}

void stop_timer() {
    timer2_on = 0;
    TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
}

int timer2_in_progress() {
    if ((timer2_state == state) && timer2_on) 
        return 1;
    else
        return 0;
}

ISR (TIMER2_COMPA_vect)
{
    if (timer2_counter >= timer2_target) {
        // HIT TOP
        timer2_counter = 0;

        go_next_state();
        stop_timer();
    }

    else {
        timer2_counter++;

    }
}


#endif