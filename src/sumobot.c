// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"


// On Start
int main() {

    // Setup timers, motors, sensors etc.    
	init();
    // Enable interrupts
    sei();

    while(1) {
      loop();
  }
}


void init() {
#if RELEASE
#endif

#if DEBUG
	initUSART();
	printString("Shadow d\n");
#endif

    prepare_debug_led();
    prepare_ping();
    prepare_line();
    prepare_motor();
    prepare_timer();

    state = START; // beginning state

    _delay_ms(3000); // total 5s delay, 2s for startup

    debug_led(0, 0, 1); // signals that the timer delay is over
}

void loop() {

    switch(state) {

        case START: // spin around and seek the enemy
        set_motor(MOTOR_FWD_HALF, MOTOR_REV_HALF);
        _delay_ms(200);
        state=SPIN;
 
        case SPIN: // seeking the enemy
        // printString("SPIN");
        set_motor(MOTOR_REV_QUARTER +4, MOTOR_FWD_QUARTER - 4);
        debug_led(1, 1, 1);
        printString("SPIN");
        break;

        case RIGHT_LINE: // move back and spin, you've hit the outside on the front right
        debug_led(0, 1, 0);
        break;

        case LEFT_LINE: // move back and spin, you've hit the outside on the front left
        debug_led(0, 1, 0);

        break;
        case FRONT_LINE: // move back and spin, you've hit the outside on both front sensors
        debug_led(0, 1, 0);

        break;
        case BACK_LINE: // charge forwards at full speed, you've been pushed to the edge!
        debug_led(0, 1, 0);

        break;

        case CHARGE_SLOW: // you've been pushing forward for a while, use physics to your advantage by slowing down and increasing friction
        printString("slow");
        set_motor(MOTOR_FWD_QUARTER, MOTOR_FWD_QUARTER);
        break;
    }
}

