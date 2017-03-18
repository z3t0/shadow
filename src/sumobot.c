// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"

int main() {
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
    #endif

    prepare_IR_sensors();
}

void loop() {
    
}
