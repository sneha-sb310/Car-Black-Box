/*
 * File:   isr.c
 * Author: Sneha
 */
#include <xc.h>
#include "timers.h"
#include "digital_keypad.h"

extern unsigned char return_time;
extern unsigned char wait_time;

// define here, extern in digital_keypad.c
volatile unsigned char long_press = 0;
volatile unsigned char held_key = ALL_RELEASED;

void __interrupt() isr(void) {

    static unsigned int count;
    static unsigned int hold_count;

    if (TMR2IF) {

        if (++count == 20000) {
            count = 0;
            if (return_time > 0) return_time--;
            if (wait_time > 0)   wait_time--;
        }

        // long press detection
        unsigned char current = PORTB & 0x3F;
        if (current != ALL_RELEASED) {
            held_key = current;
            if (++hold_count >= LONG_PRESS) {
                long_press = 1;
                hold_count = 0;
            }
        } else {
            held_key    = ALL_RELEASED;
            hold_count  = 0;
            // don't clear long_press_flag here ? let read_digital_keypad consume it
        }

        TMR2IF = 0;
    }
}