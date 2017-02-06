// sumobot.c
// Copyright (c) 2017 Rafi Khan

#include "sumobot.h"

int main() {

    while(1) {  

        #if RELEASE
        // Startup Delay of 5000ms : this number needs to be modified
        _delay_ms(5000);
        #endif

        loop();

    }
}

void loop() {
    
}