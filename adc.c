/*
 * File:   adc.c
 * Author: Sneha
 *
 */


#include <xc.h>
#include "adc.h"

void init_adc(void){
    
    //selecting right justification
    ADFM = 1;
    
    //starting the ADC module
    ADON = 1;
}

unsigned short read_adc(unsigned char channel){
    unsigned short reg_val;
    
    //selecting the required channel
    ADCON0 = (ADCON0 & 0xC7) | (channel << 3);
    
    //Start the ADC conversion
    GO = 1;
    
    //wait for the conversion to complete
    while(nDONE);   //nDONE = 0 GO = 0;
    
    reg_val = (ADRESH << 8) | ADRESL;   //11 1111 1111 -> 1023
    
    return reg_val;
    
}