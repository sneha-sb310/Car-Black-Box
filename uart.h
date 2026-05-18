/* 
 * File:   uart.h
 * Author: LENOVO
 *
 * Created on 18 March, 2026, 9:09 PM
 */

#ifndef UART_H
#define	UART_H

#define FOSC            20000000

void init_uart(unsigned long baud);     
unsigned char getchar(void);            //to receive
void putchar(unsigned char data);       //to transmit
void puts(const char *s);                //to transmit 

#endif	/* UART_H */

