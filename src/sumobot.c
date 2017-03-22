// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"

enum STATE {
    START,
    START_DELAY,
    FRONT_LINE,
    LEFT_LINE,
    BACK_LINE,
    RIGHT_LINE,
    FIND_ENEMY
};

enum STATE state = START;

// On Start
int main() {
    
    // Setup timers, motors, sensors etc.    
	init();
    // Enable interrupts
    sei();

    debug_led(1, 1, 1);
	while(1) {
		loop();
	}
}


void init() {
#if RELEASE
    // TODO: Start up Timer
    // need to figure out a way to have a 5 second "delay" including all the
    // settup so tha no time is lost
	// Startup Delay of 5000ms : this number needs to be modified
	_delay_ms(5000);
#endif

#if DEBUG
	initUSART();
	printString("Shadow Initialized\n");
#endif

    prepare_debug_led();
    // debug_led(1, 0, 0);
    // prepare_ping();
    // prepare_line();
    prepare_motor();
    OCR0B = 93;
    OCR0A = 125;
}

int a = 1;

void loop() {
    // OCR0B = a;
    // debug_led
    // switch(state) {
    //     case START:
    //         left_motor(100);
    //         right_motor(100);
    //         break;
    //     case FRONT_LINE:
    //         stop_motor();
    //         break;
    //     case LEFT_LINE:
    //         stop_motor();
    //         break;
    //     case RIGHT_LINE:
    //         stop_motor();
    //         break;
    // }
}

