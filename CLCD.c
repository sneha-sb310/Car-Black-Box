/*
 * File:   CLCD.c
 * Author: Sneha
 *
 */


#include <xc.h>
#include "CLCD.h"

void clcd_write(unsigned char byte, unsigned char mode) {
    CLCD_RS = mode;
    CLCD_DATA_PORT = byte; // byte & 0xf0

    CLCD_EN = HIGH;
    __delay_us(100);
    CLCD_EN = LOW;

    __delay_us(4100);

    /*
     
    CLCD_DATA_PORT = byte & 0xf0;
    CLCD_RS = 0;
    CLCD_EN = HIGH;
    __delay_us(100);
    CLCD_EN = LOW;
    
    CLCD_DATA_PORT = (byte & 0x0f) << 4;
     
    CLCD_EN = HIGH;
    __delay_us(100);
    CLCD_EN = LOW;

    __delay_us(4100);
     */

}

static void init_display_controller(void) {

    __delay_ms(30);

    clcd_write(EIGHT_BIT_MODE, INS_MODE); //0x33
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE, INS_MODE);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE, INS_MODE);
    __delay_us(1);

    clcd_write(TWO_LINES_5x8_8_BIT_MODE, INS_MODE);
    __delay_us(100);
    clcd_write(CLEAR_DISP_SCREEN, INS_MODE);
    __delay_us(500);
    clcd_write(DISPLAY_ON_CUR_OFF, INS_MODE);
    __delay_us(100);

}

void init_clcd(void) {

    CLCD_DATA_PORT_DDR = 0x00;

    //setting the RS and EN lines as output
    CLCD_RS_DDR = 0;
    CLCD_EN_DDR = 0;

    init_display_controller();

}

void clcd_putch(const char data, unsigned char addr) {

    clcd_write(addr, INS_MODE); //first send addr where you want to print
    clcd_write(data, DATA_MODE); //send data what you want to print;

}

void clcd_print(const char *str, unsigned char addr) {

    clcd_write(addr, INS_MODE);

    while (*str != '\0')
        clcd_write(*str++, DATA_MODE);

}

void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INS_MODE);
    __delay_us(500);
}
