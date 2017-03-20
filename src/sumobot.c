// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"

// On Start
int main() {
    
    // Setup timers, motors, sensors etc.    
	init();
    // Enable interrupts
    sei();

    debug_led(0, 0, 0);
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
    prepare_ping();
    prepare_line();
}

void loop() {
    
}
