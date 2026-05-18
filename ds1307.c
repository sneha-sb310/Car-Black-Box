/*
 * File:   ds1307.c
 * Author: Sneha
 *
 */


#include <xc.h>
#include "i2c.h"
#include "ds1307.h"
//turn of WDT
#pragma config WDTE = OFF

void init_ds1307(void){
    unsigned char dummy;
    
    dummy = read_ds1307(SEC_ADDR);
    dummy = dummy & 0x7F;
    write_ds1307(SEC_ADDR,dummy);
}

unsigned char read_ds1307(unsigned char addr){
    unsigned char data;
    
    i2c_start();
    i2c_write(SLAVE_WRITE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
    
}
void write_ds1307(unsigned char addr,unsigned char data){
        
    i2c_start();
    i2c_write(SLAVE_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
    
}