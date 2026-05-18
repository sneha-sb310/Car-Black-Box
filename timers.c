/*
 * File:   timers.c
 * Author: Sneha
 *
 */


#include <xc.h>
#include "timers.h"

void init_timer2(void){
    
    //to get 250 ticks
    PR2 = 250;
    
    //enable t2 interrupt
    TMR2IE = 1;
    
    //turn on the timer
    TMR2ON = 0;   
    
    TMR2IF = 0;
    
}