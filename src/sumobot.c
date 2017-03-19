// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"


int main() {

    prepare_ping();

	sei();

	init();

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
    if (success) {

    char c[10];
    utoa(ref, c, 10);
    printString(c);
    success = 0; 
    end = 0;
    start = 0;
    }

}
