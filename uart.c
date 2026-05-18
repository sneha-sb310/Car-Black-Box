/*
 * File:   uart.c
 * Author: LENOVO
 *
 * Created on 18 March, 2026, 9:09 PM
 */


#include <xc.h>
#include "uart.h"
//turn of WDT
#pragma config WDTE = OFF

void init_uart(unsigned long baud){
    
    //setting RC6 and RC7 to work as serial port
    SPEN = 1;
    
    //continuous reception enable bit
    CREN = 1;
    
    //baud rate setting register
    SPBRG = (FOSC / (16 * baud)) - 1;
}

unsigned char getchar(void){
    
    //wait for the byte to be received
    while(RCIF != 1)
        continue;
    
    //clear the interrupt flag
    RCIF = 0;
    
    //return the data to the caller
    return RCREG;
}
void putchar(unsigned char data){
    
    //transmit the data to the serial port
    TXREG = data;
    
    //wait till transmission is complete
    do{
        continue;
    }while(TXIF != 1);
    
    //clear the interrupt flag
    TXIF = 0;
}
void puts(const char *s){
    while(*s){
        putchar(*s++);
    }
}
