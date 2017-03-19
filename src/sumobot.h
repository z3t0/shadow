

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
#include <avr/interrupt.h>

// Libraries
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

/******************************
 * Ping Sensor
******************************/
// Ping Sensor
#define PING_TRIG PB0
#define PING_ECHO PD6
#define PING_PIN PINB
#define PING_DDR DDRB
#define PING_PORT PORTB

#define PING_MAX_CM 100

// volatile uint16_t start = 0;	// Starting tick
// volatile uint16_t end = 0; // End tick
// volatile uint16_t overflow = 0; // * 65535 (16bit)
// volatile uint16_t edge = 0; // 0 = rising, 1 = falling, 2 =done
// volatile uint16_t last = 0; // latest distance in cm


// /// Timer 1 Edge interrupt:
// //	Saves rising and falling edge
// ISR(TIMER1_CAPT_vect) {
// 	// Read 16 bit val from ICR
// 	uint16_t val = ICR1L;
// 	val |= ICR1H << 8;

// 	if (edge == 0) { // Rising edge detected
// 		start = val;
// 		edge = 1;
// 		overflow = 0;

// 		TCCR1B &= ~_BV(ICES1); // Now look for falling edge
// 	}

// 	else if (edge == 1){ // Falling edge detected
// 		end = val;

// 		// convert to cm
// 		uint16_t temp  = ((end + (overflow * 65535) - start) / (float)65535 * .265 * 1000000 / 58);
// 		if (temp > PING_MAX_CM)
// 			last = PING_MAX_CM;
// 		else
// 			last = temp;
// 		edge = 3;	// Success
// 	}
// }

// /// Overflow interrupt
// //	increments counter
// ISR(TIMER1_OVF_vect){
// 	overflow++;
// }

volatile uint16_t ref;
volatile uint16_t success = 0;
volatile uint16_t overflow = 0;
volatile uint16_t end = 0;

void prepare_ping() {
    // Sensor
    DDRD |= _BV(PD7); //PD7 as output
    PORTD &= ~_BV(PD7); //PD7 to LOW
    DDRB &= ~_BV(PB0); //PB0 as input
    PORTB &= ~_BV(PB0); //PB0 no pullup

    // Timer
    TCNT1 = 0; //Clear TIMER1 value
    OCR1A = 159; //5us when TIMER1 is divided by 8
    TIMSK1 = _BV(OCIE1A); //Enable compare match interrupt
    TCCR1B = _BV(ICES1) | _BV(CS10); //Trigger input capture on rising edge, divide TIMER1 by 8 and start TIMER1

}


ISR(TIMER1_COMPA_vect)
{
    if(PORTD&(1<<PD7)) //Test if query pulse on
    {
        PORTD&=~(1<<PD7); //End query pulse
        TIMSK1=(1<<ICIE1) | (1<<TOIE1); //Disable compare match interrupt and enable input capture interrupt
    }
    else //Query pulse not started
    {
        PORTD|=(1<<PD7); //Start query pulse
    }
    TCNT1=0; //Clear TIMER1 value
}

ISR(TIMER1_CAPT_vect)
{
    TCCR1B&=~(1<<CS10); //Stop TIMER1
    if(PINB&(1<<PB0)) //Test if response pulse started
    {
        TCNT1 = 0;
        overflow = 0;
        TCCR1B&=~(1<<ICES1); //Next timer trigger on falling edge
    }
    else //Response pulse ended
    {
        end = TCNT1;
        ref = ((end * (float) 0.0625) / 58 + (overflow * 70.62));
        success = 1;
        TIMSK1=(1<<OCIE1A); //Disable input capture interrupt and enable compare match interrupt
        TCCR1B|=(1<<ICES1); //Next timer trigger on rising edge
    }
    TCNT1=0; //Clear TIMER1 value
    TCCR1B|=(1<<CS10); //Start TIMER1
}

ISR(TIMER1_OVF_vect) {
    overflow++;
}

void read_ping() {
    // Pulse to high or pulse to low?

    // Pulse out : 5us
    PING_DDR |= _BV(PING_TRIG); // OUTPUT
    PING_PORT |= _BV(PING_TRIG);

    _delay_us(5);

    // Pulse in 
    // PING_PORT &= ~_BV(PING_TRIG);
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