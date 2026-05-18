/*
 * File:   digital_keypad.c
 * Author: Sneha
 *
 */


#include <xc.h>
#include "digital_keypad.h"


extern volatile unsigned char long_press;
extern volatile unsigned char held_key;

void init_digital_keypad(void){
    TRISB = TRISB | 0x3f;
}
/*
unsigned char read_digital_keypad(unsigned char mode){
    //detect key and return key level/edge triggering
    static unsigned char once = 1;
    
    if(mode == LEVEL)
        return (PORTB & 0x3f);
    else if(mode == STATE){
        if((PORTB & 0x3F) != ALL_RELEASED && once){
            once = 0;
            return (PORTB & 0x3f);
        }
        else if((PORTB & 0x3F) == ALL_RELEASED){
            once = 1;
        }
    }
    return ALL_RELEASED;
}
*/

unsigned char read_digital_keypad(unsigned char mode) {
    static unsigned char once = 1;
    static unsigned char long_press_flag = 0;
    static unsigned char prev_key = ALL_RELEASED;
    
    unsigned char current = PORTB & 0x3F;

    if (mode == LEVEL)
        return current;

    else if (mode == STATE) {

        // key just pressed ? buffer it, don't return yet
        if (current != ALL_RELEASED && once) {
            once = 0;
            prev_key = current;   // remember which key was pressed
        }

        if (current != ALL_RELEASED && once) {
            if (current == SW1 || current == SW2 || 
                current == SW3 || current == SW6) {
                once = 0;
                return current;   // immediate return, no buffering
            }
        }

        // SW4, SW5 ? could be long press, buffer and decide on release
        if ((current == SW4 || current == SW5) && once) {
            once = 0;
            prev_key = current;
        }
        
        // key released ? now decide what to return
        if (current == ALL_RELEASED && !once) {
            once = 1;

            if (long_press_flag) {
                // long press already returned ? suppress short press
                long_press_flag = 0;
                prev_key = ALL_RELEASED;
                return ALL_RELEASED;
            }

            // key released without long press return as short press
            unsigned char k = prev_key;
            prev_key = ALL_RELEASED;
            return k;
        }

        if (long_press && !long_press_flag) {
            long_press = 0;
            long_press_flag = 1;
            switch (held_key) {
                case SW4: return L_SW4;
                case SW5: return L_SW5;
                default:  return ALL_RELEASED;
            }
        }
    }
    return ALL_RELEASED;
}