/* 
 * File:   digital_keypad.h
 * Author: Sneha
 *
 */

#ifndef DIGITAL_KEYPAD_H
#define	DIGITAL_KEYPAD_H

#define LEVEL 0
#define STATE 1

#define SW1     0x3E
#define SW2     0x3D
#define SW3     0x3B
#define SW4     0x37
#define SW5     0x2F
#define SW6     0x1F

#define L_SW4   0XB7
#define L_SW5   0xAF

#define ALL_RELEASED  0x3F

#define LONG_PRESS  10000  // tune this value


void init_digital_keypad(void);
unsigned char read_digital_keypad(unsigned char mode);

#endif	/* DIGITAL_KEYPAD_H */