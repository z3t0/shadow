// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"
#include <avr/interrupt.h>

volatile uint16_t start = 0;
volatile uint16_t end = 0;
volatile uint16_t overflow = 0;
volatile uint16_t edge = 0;

void pulse() {
	PING_DDR |= _BV(PING_INT);
	PING_PORT |= _BV(PING_INT);
	_delay_us(5);
	PING_PORT &= ~_BV(PING_INT);
	PING_DDR &= ~_BV(PING_INT); _delay_us(2);
}

void init_timer1() {
	TCCR1B |= ((1 << CS11) | (1 << CS10)); //64 prescaler
	TIMSK1 |= _BV(ICIE1) |_BV(TOIE1); // Enable overflow and edge interrupt
	TCCR1B |= _BV(ICES1); // interrupt on rising edge
}

void stop_timer() {
	TCCR1B &= ~((1 << CS11) | (1 << CS10)); //64 prescaler
}

int main() {
	init_timer1();

	sei();

	init();

	/* pulse(); */


	while(1) {
		loop();
	}
}


void init() {
#if RELEASE
	// Startup Delay of 5000ms : this number needs to be modified
	_delay_ms(5000);
#endif

#if DEBUG
	prepare_debug_led();
	initUSART();
	printString("Shadow Initialized\n");
#endif

	prepare_IR_sensors();
}

void loop() {
	if (edge == 3) {
		char c1[10];
		char c2[10];
		char c3[10];

		utoa(overflow, c1, 10);
		utoa(start, c2, 10);
		utoa(end, c3, 10);

		uint16_t t = (end + (overflow * 65535) - start) / (float)65535 * .265 * 1000;


		char buffer[10];
		utoa(t, buffer, 10);

		printString("time");
		printString(buffer);
		printString("overflow");
		printString(c1);
		printString("start");
		printString(c2);
		printString("end");
		printString(c3);

		TCCR1B |= _BV(ICES1); // interrupt on rising edge
		edge = 0;
		start = 0;
		end = 0;
		overflow = 0;

		/* _delay_ms(1); */
		/* pulse(); */
	}
}

ISR(TIMER1_CAPT_vect) {
	// Read 16 bit val
	uint16_t val = ICR1L;
	val |= ICR1H << 8;

	if (edge == 0) {
		start = val;
		edge = 1;
		overflow = 0;

		TCCR1B &= ~_BV(ICES1); // falling edge
	}

	else if (edge == 1){
		end = val;
		edge = 3;
	}

}

ISR(TIMER1_OVF_vect){
	overflow++;
}
